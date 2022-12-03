#include <RollingRaspberry/vision/vision_module.h>

VisionModule::VisionModule(CameraStream* stream, AprilTagDetectorSettings settings)
: cam_stream(stream), tag_detector(settings) { }

VisionModule::~VisionModule() {
  if (!is_terminated()) {
    terminate();
    module_thread.join();
  }
}

void VisionModule::init_thread() {
  thread_running = true;
  thread_terminated = false;
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
    
    if (frame_time) {
      cv::cvtColor(frame, frame_gray, cv::COLOR_RGB2GRAY);
      
      std::vector<AprilTagDetector::Detection> detections = tag_detector.detect(frame_gray);
      
      cs::CvSource* output_stream = cam_stream->get_ouput_source();
      
      if (output_stream) {
        for (auto& det : detections) {
          tag_detector.visualize_detection(frame, det);
        }
        
        output_stream->PutFrame(frame);
      }
    }
    else {
      fmt::print(FRC1511_LOG_PREFIX "{}: Error grabbing frame: {}\n", cam_stream->get_name(), cam_stream->get_error());
    }
    
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
