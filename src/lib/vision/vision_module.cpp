#include <RollingRaspberry/vision/vision_module.h>
#include <frc/apriltag/AprilTagDetector_cv.h>

VisionModule::VisionModule(CameraStream* stream, const VisionSettings* settings)
: cam_stream(stream), cam_model(cam_stream->get_props().model), settings(settings),
  pose_estimator(frc::AprilTagPoseEstimator::Config { settings->tag_size,
    cam_model->get_fx(), cam_model->get_fy(),
    cam_model->get_cx(), cam_model->get_cy(),
  }) {

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

std::chrono::high_resolution_clock::time_point VisionModule::get_pose_estimate_time_point() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return pose_estimate_time_point;
}

bool VisionModule::has_new_pose_estimate() const {
  std::lock_guard<std::mutex> lk(module_mutex);
  return new_pose_estimate;
}

frc::AprilTagPoseEstimate VisionModule::get_pose_estimate() {
  std::lock_guard<std::mutex> lk(module_mutex);
  new_pose_estimate = false;
  return pose_estimate;
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

        // Set the pose estimate.
        {
          std::lock_guard<std::mutex> lk(module_mutex);
          pose_estimate = std::move(est);
          pose_estimate_time_point = std::chrono::high_resolution_clock::now();
          new_pose_estimate = true;
        }
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

  std::string text = std::to_string(det.GetId());
  int font_face = cv::FONT_HERSHEY_SIMPLEX;
  double font_scale = 1.0;
  int base_line;
  cv::Size text_size = cv::getTextSize(text, font_face, font_scale, 2, &base_line);
  cv::putText(frame, text, cv::Point(det.GetCenter().x - text_size.width/2, det.GetCenter().y + text_size.height/2), font_face, font_scale, cv::Scalar(255, 153, 0), 2);
}
