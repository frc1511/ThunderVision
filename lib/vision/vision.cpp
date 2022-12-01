#include <RollingRaspberry/vision/vision.h>

VisionModule::VisionModule(CameraStream* stream)
: cam_stream(stream) {
  
  module_thread = std::thread([&]() {
    thread_start();
  });
}

VisionModule::~VisionModule() {
  terminate();
  module_thread.join();
}

VisionModule::VisionModule(VisionModule&& other) {
  terminate();
  module_thread.join();
  
  other.cam_stream = cam_stream;
}

void VisionModule::set_running(bool running) {
  std::lock_guard<std::mutex> lk(module_mutex);
  thread_running = running;
}

void VisionModule::terminate() {
  std::lock_guard<std::mutex> lk(module_mutex);
  thread_terminated = true;
}

void VisionModule::thread_start() {
  using namespace std::chrono_literals;
  
  std::chrono::high_resolution_clock::time_point start, end;
  
  apriltag_family_t* tag_family(tag16h5_create());
  apriltag_detector_t* tag_detector(apriltag_detector_create());
  
  // Tell the detector which family of tags to detect.
  apriltag_detector_add_family(tag_detector, tag_family);
  
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
    }
    
    std::uint64_t frame_time = cam_stream->get_frame(frame);
    
    if (frame_time) {
      cv::cvtColor(frame, frame_gray, cv::COLOR_RGB2GRAY);
      
      image_u8_t im = { frame_gray.cols, frame_gray.rows, frame_gray.cols, frame_gray.data };
      
      zarray_t* detections = apriltag_detector_detect(tag_detector, &im);
      
      cs::CvSource* output_stream = cam_stream->get_ouput_source();
      
      if (output_stream) {
        for (int i = 0; i < zarray_size(detections); i++) {
          apriltag_detection_t* det;
          zarray_get(detections, i, &det);
          cv::line(frame, cv::Point(det->p[0][0], det->p[0][1]), cv::Point(det->p[1][0], det->p[1][1]), cv::Scalar(0, 0xff, 0), 2);
          cv::line(frame, cv::Point(det->p[0][0], det->p[0][1]), cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(0, 0, 0xff), 2);
          cv::line(frame, cv::Point(det->p[1][0], det->p[1][1]), cv::Point(det->p[2][0], det->p[2][1]), cv::Scalar(0xff, 0, 0), 2);
          cv::line(frame, cv::Point(det->p[2][0], det->p[2][1]), cv::Point(det->p[3][0], det->p[3][1]), cv::Scalar(0xff, 0, 0), 2);
          
          std::stringstream ss;
          ss << det->id;
          std::string text = ss.str();
          int fontface = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
          double fontscale = 1.0;
          int baseline;
          cv::Size textsize = cv::getTextSize(text, fontface, fontscale, 2, &baseline);
          cv::putText(frame, text, cv::Point(det->c[0]-textsize.width/2, det->c[1]+textsize.height/2), fontface, fontscale, cv::Scalar(0xff, 0x99, 0), 2);
        }
      
        output_stream->PutFrame(frame);
      }
      
      apriltag_detections_destroy(detections);
    }
    else {
      fmt::print("FRC1511: Error grabbing frame from {}\n", cam_stream->get_desc());
    }
    
    end = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001;
    double period = 1.0 / cam_stream->get_props().fps;

    if (dur < period) {
      std::size_t sleep_time = (period - dur) * 1000.0;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
  }
  
  // Clean up April Tag stuff...
  apriltag_detector_destroy(tag_detector);
  tag16h5_destroy(tag_family);
}

Vision::Vision(Settings& settings) 
: usb_cameras(settings.get_usb_cameras()),
  mjpg_cameras(settings.get_mjpg_cameras()) {
  
  vision_modules.reserve(usb_cameras.size() + mjpg_cameras.size());
  
  for (auto& cam : usb_cameras) {
    vision_modules.emplace_back(&cam);
  }
  for (auto& cam : mjpg_cameras) {
    vision_modules.emplace_back(&cam);
  }
}

Vision::~Vision() { }

void Vision::reset_to_mode(NTHandler::MatchMode mode) { }

void Vision::process() { }

