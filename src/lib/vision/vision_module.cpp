#include <RollingRaspberry/vision/vision_module.h>
#include <frc/apriltag/AprilTagDetector_cv.h>

VisionModule::VisionModule(CameraStream* stream, const VisionSettings* settings)
: settings(settings), cam_stream(stream), cam_props(&cam_stream->get_props()),
  robot_to_camera(cam_props->robot_to_camera),

  // Adjust the camera matrix to account for the resolution of the camera.
  cam_matrix(cam_props->model->get_adjusted_matrix(cam_props->width, cam_props->height)),

  // Initialize the AprilTagPoseEstimator with the intrinsic properties of the camera.
  pose_estimator(frc::AprilTagPoseEstimator::Config { settings->tag_size,
    cam_matrix[0], cam_matrix[4], // fx, fy
    cam_matrix[2], cam_matrix[5], // cx, cy
  }) {

  // Set the family of AprilTags to detect.
  tag_detector.AddFamily(settings->tag_family);
}

VisionModule::~VisionModule() {
  if (!is_terminated()) {
    terminate();
    module_thread.join();
  }
}

void VisionModule::init_thread() {
  // Ready...
  thread_running = true;

  // Set...
  thread_terminated = false;

  // GOOOOOO!!!!!!
  module_thread = std::thread([&]() {
    thread_start();
  });
}

void VisionModule::set_running(bool running) {
  std::lock_guard<std::mutex> lk(module_mutex);
  thread_running = running;
}

bool VisionModule::is_running() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return thread_running;
}

bool VisionModule::is_terminated() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return thread_terminated;
}

std::chrono::high_resolution_clock::time_point VisionModule::get_pose_estimates_time_point() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return pose_estimates_time_point;
}

bool VisionModule::has_new_pose_estimates() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return new_pose_estimates;
}

std::vector<frc::AprilTagPoseEstimate> VisionModule::get_pose_estimates() {
  std::lock_guard<std::mutex> lk(module_mutex);
  new_pose_estimates = false;
  return pose_estimates;
}

frc::Transform3d VisionModule::get_robot_to_camera_transform() const {
  return robot_to_camera;
}

void VisionModule::terminate() {
  std::lock_guard<std::mutex> lk(module_mutex);
  thread_terminated = true;
}

void VisionModule::thread_start() {
  using namespace std::chrono_literals;
  
  std::chrono::high_resolution_clock::time_point start, end;
  
  cv::Mat frame, frame_gray;
  
  while (true) {
    start = std::chrono::high_resolution_clock::now();
    
    bool running = false;
    {
      std::lock_guard<std::mutex> lk(module_mutex);
      if (thread_terminated) break;
      running = thread_running;
    }
    
    if (!running) {
      // Try again in 1 second.
      std::this_thread::sleep_for(1s);
      continue;
    }
    
    std::uint64_t frame_time = cam_stream->get_frame(frame);
    
    // Successfully grabbed a frame.
    if (frame_time) {
      cv::cvtColor(frame, frame_gray, cv::COLOR_RGB2GRAY);
      
      // Detect tags.
      frc::AprilTagDetector::Results results(frc::AprilTagDetect(tag_detector, frame_gray));

      cs::CvSource* output_stream = cam_stream->get_ouput_source();

      std::vector<frc::AprilTagPoseEstimate> working_pose_estimates;

      for (const frc::AprilTagDetection* det : results) {
        // Skip detections with low decision margin.
        if (det->GetDecisionMargin() < settings->min_decision_margin) continue;

        // Visualize detection during debug.
#ifndef NDEBUG
        if (output_stream) {
          visualize_detection(frame, *det);
        }
#endif

        // Estimate the pose of the tag.
        frc::AprilTagPoseEstimate est(pose_estimator.EstimateOrthogonalIteration(*det, settings->estimate_iters));

        working_pose_estimates.push_back(est);
      }

      if (!working_pose_estimates.empty()) {
        std::lock_guard<std::mutex> lk(module_mutex);
        pose_estimates = working_pose_estimates;
        pose_estimates_time_point = start;
        new_pose_estimates = true;
      }
      
      output_stream->PutFrame(frame);
    }
    else {
      fmt::print(FRC1511_LOG_PREFIX "{}: Error grabbing frame: {}\n", cam_stream->get_name(), cam_stream->get_error());
    }
    
    // Sleep for the remainder of the frame period.

    end = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001;
    double period = 1.0 / cam_stream->get_props().fps;

    if (dur < period) {
      std::size_t sleep_time = (period - dur) * 1000.0;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
  }

  fmt::print(FRC1511_LOG_PREFIX "{}: Vision module thread terminated\n", cam_stream->get_name());
}

void VisionModule::visualize_detection(cv::Mat& frame, const frc::AprilTagDetection& det) {
  cv::Point c0 = cv::Point(det.GetCorner(0).x, det.GetCorner(0).y);
  cv::Point c1 = cv::Point(det.GetCorner(1).x, det.GetCorner(1).y);
  cv::Point c2 = cv::Point(det.GetCorner(2).x, det.GetCorner(2).y);
  cv::Point c3 = cv::Point(det.GetCorner(3).x, det.GetCorner(3).y);

  cv::line(frame, c2, c3, cv::Scalar(0, 255, 255), 2); // Top
  cv::line(frame, c0, c1, cv::Scalar(0, 255, 0),   2); // Bottom
  cv::line(frame, c1, c2, cv::Scalar(255, 0, 0),   2); // Left
  cv::line(frame, c3, c0, cv::Scalar(0, 0, 255),   2); // Right

  std::string text = fmt::format("{}", det.GetId());
  int font_face = cv::FONT_HERSHEY_SIMPLEX;
  double font_scale = 0.5;
  int base_line;
  cv::Size text_size = cv::getTextSize(text, font_face, font_scale, 2, &base_line);
  // Center the text in the tag.
  cv::putText(frame, text, cv::Point(det.GetCenter().x - text_size.width / 2, det.GetCenter().y + text_size.height / 2), font_face, font_scale, cv::Scalar(255, 153, 0), 2);
}
