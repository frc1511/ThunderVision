#include <RollingRaspberry/vision/vision_settings.h>

void to_json(wpi::json& json, const VisionSettings& settings) {
  json = wpi::json{
    {"usb_cameras", settings.usb_camera_props},
    {"mjpg_cameras", settings.mjpg_camera_props},
    {"apriltag_detector", settings.apriltag_detector_settings}
  };
}

void from_json(const wpi::json& json, VisionSettings& settings) {
  // USB Camera list.
  settings.usb_camera_props = json.at("usb_cameras").get<std::vector<USBCameraProps>>();

  // MJPG Stream list.
  settings.mjpg_camera_props = json.at("mjpg_cameras").get<std::vector<MJPGCameraProps>>();

  // AprilTag Detector Settings.
  settings.apriltag_detector_settings = json.at("apriltag_detector").get<AprilTagDetectorSettings>();
}