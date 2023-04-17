#include <RollingRaspberry/vision/vision_module.h>
#include <frc/apriltag/AprilTagDetector_cv.h>
#include <RollingRaspberry/network/nt_handler.h>

static const std::filesystem::path RECORDING_PATH("/media/usb/recordings");

VisionModule::VisionModule(CameraStream* stream, const VisionSettings* settings)
: settings(settings), cam_stream(stream), cam_props(&cam_stream->get_props()),
  robot_to_camera(cam_props->robot_to_camera),

  int_mat(cam_props->model->get_adjusted_intrinsic_matrix(cam_props->width, cam_props->height)),
  pose_estimator(frc::AprilTagPoseEstimator::Config { settings->tag_size, int_mat[0], int_mat[4], int_mat[2], int_mat[5] }) {

  // Set the family of AprilTags to detect.
  tag_detector.AddFamily(settings->tag_family);

  time_t now = time(0);
  auto* curr_time = localtime(&now);

  // month_day_year__hour_min
  std::string date = fmt::format("{}_{}_{}__{}_{}", curr_time->tm_mon + 1, curr_time->tm_mday, curr_time->tm_year + 1900, curr_time->tm_hour, curr_time->tm_min);

  // When running as a systemd service, current_path() is root.
  image_path = RECORDING_PATH / date / cam_props->name;
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

bool VisionModule::has_new_detections() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return new_detections;
}

std::map<Clock::TimePoint, std::pair<int, frc::AprilTagPoseEstimate>> VisionModule::get_detections() {
  std::lock_guard<std::mutex> lk(module_mutex);
  new_detections = false;
  return detections;
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
  
  Clock::TimePoint start;
  
  cv::Mat frame, frame_gray, frame_undistorted;

  while (true) {
    start = Clock::get_time_point();
    
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
    
    if (cam_stream->should_record()) {
      bool was_usb_device_connected = usb_device_connected;
      usb_device_connected = std::filesystem::exists(RECORDING_PATH);
      if (!was_usb_device_connected && usb_device_connected) {
        std::filesystem::create_directories(image_path);
      }
      if (!usb_device_connected) {
        fmt::print("Valid USB device not mounted.\n");
      }
    }
    
    std::uint64_t frame_res = cam_stream->get_frame(frame_undistorted);
    Clock::TimePoint frame_time_point = Clock::get_time_point();
    
    // Successfully grabbed a frame.
    if (frame_res) {
      // Save current frame to image file.
      if (cam_stream->should_record()) {
        std::string filename = fmt::format("{}/{}.jpg", image_path.string(), image_num++);
        
        if (usb_device_connected) {
          cv::imwrite(filename, frame_undistorted, quality_params);
        }
      }

      // Output stream (optional - may be nullptr).
      cs::CvSource* output_stream = cam_stream->get_ouput_source();

      // Run AprilTag detection.
      if (cam_stream->should_run_detection()) {
        cv::cvtColor(frame_undistorted, frame_gray, cv::COLOR_RGB2GRAY);
        
        // Detect tags.
        frc::AprilTagDetector::Results results(frc::AprilTagDetect(tag_detector, frame_gray));
        
        std::map<Clock::TimePoint, std::pair<int, frc::AprilTagPoseEstimate>> working_detections;
        
        for (const frc::AprilTagDetection* det : results) {
          // Skip detections with low decision margin.
          if (det->GetDecisionMargin() < settings->min_decision_margin) continue;
          
          // Visualize detection during debug.
#ifndef NDEBUG
          if (output_stream) {
            visualize_detection(frame_undistorted, *det);
          }
#endif
          
          // Estimate the pose of the tag.
          frc::AprilTagPoseEstimate est(pose_estimator.EstimateOrthogonalIteration(*det, settings->estimate_iters));
          
          working_detections.emplace(frame_time_point, std::make_pair(det->GetId(), est));
        }
        
        if (!working_detections.empty()) {
          std::lock_guard<std::mutex> lk(module_mutex);
          // Don't overwrite detections that haven't been processed yet.
          if (new_detections) {
            detections.insert(working_detections.begin(), working_detections.end());
          }
          else {
            detections = working_detections;
          }
          new_detections = true;
        }
      }
      
      // Host image.
      if (output_stream) {
        output_stream->PutFrame(frame_undistorted);
      }
    }
    else {
      fmt::print(FRC1511_LOG_PREFIX "{}: Error grabbing frame: {}\n", cam_stream->get_name(), cam_stream->get_error());
    }
    
    // Sleep for the remainder of the frame period.

    units::second_t dur = Clock::get_time_since_point(start);
    units::second_t period = 1_s / cam_stream->get_props().fps;
    
    if (dur < period) {
      std::this_thread::sleep_for(std::chrono::milliseconds((long long)(period - dur).value() * 1000));
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
