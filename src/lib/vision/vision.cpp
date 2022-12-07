#include <RollingRaspberry/vision/vision.h>

Vision::Vision() 
: vision_settings(robot_settings.get_vision_settings()) {
  
  AprilTagDetectorSettings tag_settings;//(settings->get_tag_settings());

  for (const auto& props : vision_settings.usb_camera_props) {
    usb_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&usb_cameras.back(), tag_settings));
    vision_modules.back()->init_thread();
  }
  for (const auto& props : vision_settings.mjpg_camera_props) {
    mjpg_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&mjpg_cameras.back(), tag_settings));
    vision_modules.back()->init_thread();
  }
}

Vision::~Vision() = default;

void Vision::reset_to_mode(NTHandler::MatchMode mode) { }

void Vision::process() { }

