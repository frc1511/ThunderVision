#include <thunder_vision/vision/vision.hpp>

#include <thunder_vision/diagnostics.hpp>
#include <thunder_vision/networking/networking.hpp>
#include <thunder_vision/vision/camera_stream.hpp>
#include <thunder_vision/vision/vision_config.hpp>

#include <frc/apriltag/AprilTagDetection.h>
#include <frc/apriltag/AprilTagDetector.h>
#include <frc/apriltag/AprilTagDetector_cv.h>
#include <frc/apriltag/AprilTagPoseEstimator.h>

#include <chrono>
#include <cstdio>
#include <memory>

Vision::Vision(const VisionConfig& config,
               const frc::AprilTagFieldLayout& field_layout,
               Networking& networking)
  : m_config(config),
    m_field_layout(field_layout),
    m_networking(networking) {

  setup_modules();
}

Vision::~Vision() {
  {
    std::lock_guard<std::mutex> lk(m_terminated_mutex);
    m_terminated = true;
  }

  for (std::thread& vision_module : m_vision_modules) {
    vision_module.join();
  }
}

void Vision::process() {
  const std::vector<Detection> detections = ({
    std::lock_guard<std::mutex> lk(m_detection_mutex);
    const std::vector<Detection> detections = std::move(m_detections);
    m_detections.clear();
    detections;
  });

  std::vector<frc::Pose3d> robot_poses;

  for (const Detection& detection : detections) {
    const std::optional<frc::Pose3d> robot_pose =
        calculate_robot_pose(detection);

    if (robot_pose) robot_poses.push_back(*robot_pose);
  }

  // NOTE: Send even if empty to verify that the client is still connected!
  m_networking.send_poses(robot_poses);
}

void Vision::setup_modules() {
  m_vision_modules.reserve(m_config.modules.size());

  for (const VisionModuleConfig& module_config : m_config.modules) {
    CameraStream* camera_stream =
        load_camera_stream(module_config, m_config.camera_models);

    m_vision_modules.emplace_back([this, camera_stream, module_config]() {
      this->vision_module(camera_stream, module_config);
    });
  }
}

std::optional<frc::Pose3d>
Vision::calculate_robot_pose(const Detection& detection) const {
  const auto& [tag_id, module_config, estimate] = detection;

  const frc::Pose3d tag_pose = ({
    std::optional<frc::Pose3d> tag_pose = m_field_layout.GetTagPose(tag_id);
    if (!tag_pose) return std::nullopt;
    tag_pose.value();
  });

  const auto& transform_pose = [&](const frc::Transform3d& camera_to_tag) {
    const frc::Transform3d robot_to_tag =
        module_config.robot_to_camera + camera_to_tag;
    const frc::Transform3d tag_to_robot = robot_to_tag.Inverse();

    return tag_pose + tag_to_robot;
  };

  const frc::Pose3d pose_1 = transform_pose(estimate.pose1);
  const frc::Pose3d pose_2 = transform_pose(estimate.pose2);

  const bool is_ambiguous = estimate.GetAmbiguity() > 0.2;

  // Not ambiguous.
  if (!is_ambiguous) {
    frc::Pose3d good_pose = estimate.error1 < estimate.error2 ? pose_1 : pose_2;

    /* if (validate_robot_pose(good_pose)) return good_pose; */
    /* return std::nullopt; */

    return good_pose;
  }

  // Ambiguous D:

  printf(VISION_TAG "Ambiguous pose for tag %d\n", tag_id);

  bool pose_1_valid = validate_robot_pose(pose_1);
  bool pose_2_valid = validate_robot_pose(pose_2);

  // Both bad.
  if (!pose_1_valid && !pose_2_valid) return std::nullopt;

  // Both okay.
  if (pose_1_valid && pose_2_valid) {
    const double pose_1_error = estimate.error1;
    const double pose_2_error = estimate.error2;

    return pose_1_error < pose_2_error ? pose_1 : pose_2;
  }

  // One okay.
  return pose_1_valid ? pose_1 : pose_2;
}

bool Vision::validate_robot_pose(const frc::Pose3d& robot_pose) const {
  const units::meter_t max_elevation = m_config.max_elevation;
  const units::meter_t pose_tolerance = m_config.pose_tolerance;

  if (robot_pose.Translation().Z() > max_elevation + pose_tolerance)
    return false;

  if (robot_pose.Translation().Z() < -pose_tolerance) return false;

  return true;
}

static void visualize_detection(cv::Mat& frame,
                                const frc::AprilTagDetection& detection);

void Vision::vision_module(CameraStream* _camera_stream,
                           const VisionModuleConfig& module_config) {
  std::unique_ptr<CameraStream> camera_stream(_camera_stream);
  cs::CvSource* output = camera_stream->output();

  // Construct AprilTag pose estimator.
  const std::array<double, 9> intrinsics = camera_stream->model().intrinsics;
  const frc::AprilTagPoseEstimator::Config pose_estimator_config {
      m_config.tag_size,                // AprilTag size.
      intrinsics[0],     intrinsics[4], // fx, fy
      intrinsics[2],     intrinsics[5], // cx, cy
  };
  frc::AprilTagPoseEstimator pose_estimator(pose_estimator_config);

  // Construct AprilTag detector.
  frc::AprilTagDetector detector;
  detector.AddFamily(m_config.tag_family);

  using namespace std::chrono_literals;
  const auto desired_frame_duration =
      1s / static_cast<double>(camera_stream->fps());

  cv::Mat frame_original, frame_gray;
  // Loop.
  while (true) {
    {
      std::lock_guard<std::mutex> lk(m_terminated_mutex);
      if (m_terminated) break;
    }

    const auto frame_start = std::chrono::steady_clock::now();

    //
    // Get the frame.
    //
    const uint64_t frame_res = camera_stream->get_frame(frame_original);
    if (!frame_res) {
      printf(VISION_TAG "Failed to get frame from %s: %s\n",
             camera_stream->name(), camera_stream->error().c_str());
      continue;
    }

    //
    // AprilTag detection.
    //
    if (module_config.detect_apriltags) {
      cv::cvtColor(frame_original, frame_gray, cv::COLOR_BGR2GRAY);

      const frc::AprilTagDetector::Results results =
          frc::AprilTagDetect(detector, frame_gray);

      for (const frc::AprilTagDetection* detection : results) {
        if (detection->GetDecisionMargin() < m_config.min_decision_margin)
          continue;

        if (output) visualize_detection(frame_original, *detection);

        //
        // Pose estimation.
        //
        const frc::AprilTagPoseEstimate estimate =
            pose_estimator.EstimateOrthogonalIteration(
                *detection, m_config.pose_estimate_iterations);

        {
          std::lock_guard<std::mutex> lk(m_detection_mutex);
          m_detections.push_back(
              {detection->GetId(), module_config, std::move(estimate)});
        }
      }
    }

    if (output) {
      output->PutFrame(frame_original);
    }

    // Handle FPS because camera/sink doesn't do that for us...
    const auto frame_end = std::chrono::steady_clock::now();
    const auto frame_duration = frame_start - frame_end;

    if (frame_duration < desired_frame_duration) {
      const auto sleep_time = desired_frame_duration - frame_duration;
      std::this_thread::sleep_for(sleep_time);
    }
  }
}

static void visualize_detection(cv::Mat& frame,
                                const frc::AprilTagDetection& detection) {
  const cv::Point c0 =
      cv::Point(detection.GetCorner(0).x, detection.GetCorner(0).y);
  const cv::Point c1 =
      cv::Point(detection.GetCorner(1).x, detection.GetCorner(1).y);
  const cv::Point c2 =
      cv::Point(detection.GetCorner(2).x, detection.GetCorner(2).y);
  const cv::Point c3 =
      cv::Point(detection.GetCorner(3).x, detection.GetCorner(3).y);

  cv::line(frame, c2, c3, cv::Scalar(255, 0, 0), 2);   // Top
  cv::line(frame, c0, c1, cv::Scalar(0, 255, 255), 2); // Bottom
  cv::line(frame, c1, c2, cv::Scalar(0, 255, 0), 2);   // Left
  cv::line(frame, c3, c0, cv::Scalar(255, 0, 255), 2); // Right

  const std::string id = std::to_string(detection.GetId());

  const int font_face = cv::FONT_HERSHEY_SIMPLEX;
  const double font_scale = 0.5;
  int baseline;
  const cv::Size text_size =
      cv::getTextSize(id, font_face, font_scale, 2, &baseline);

  const cv::Point text_origin(detection.GetCenter().x - text_size.width / 2.0f,
                              detection.GetCenter().y +
                                  text_size.height / 2.0f);

  cv::putText(frame, id, text_origin, font_face, font_scale,
              cv::Scalar(255, 153, 0), 2);
}

