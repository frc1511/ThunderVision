#include <RollingRaspberry/vision/vision.h>

Vision::Vision() 
: vision_settings(robot_settings.get_vision_settings()) {

  for (const auto& props : vision_settings.usb_camera_props) {
    usb_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&usb_cameras.back(), vision_settings.apriltag_detector_settings));
  }
  for (const auto& props : vision_settings.mjpg_camera_props) {
    mjpg_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&mjpg_cameras.back(), vision_settings.apriltag_detector_settings));
  }

  for (auto& mod : vision_modules) {
    mod->init_thread();
  }
}

Vision::~Vision() = default;

void Vision::reset_to_mode(NTHandler::MatchMode mode) { }

void Vision::process() { }

