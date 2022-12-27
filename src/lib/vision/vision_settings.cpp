#include <RollingRaspberry/vision/vision_settings.h>

void to_json(wpi::json& json, const VisionSettings& settings) {
  json = wpi::json{
    {"apriltag_family", settings.tag_family},
    {"apriltag_size", settings.tag_size.value()},
    {"field_layout", settings.field_layout_name},
    {"min_decision_margin", settings.min_decision_margin},
    {"usb_cameras", settings.usb_camera_props},
    {"mjpg_cameras", settings.mjpg_camera_props},
  };
}

void from_json(const wpi::json& json, VisionSettings& settings) {
  // AprilTag Family.
  settings.tag_family = json.at("apriltag_family").get<std::string>();

  // AprilTag Size.
  settings.tag_size = units::meter_t(json.at("apriltag_size").get<double>());

  // Field Layout.
  settings.field_layout_name = json.at("field_layout").get<std::string>();

  // Minimum Decision Margin.
  settings.min_decision_margin = json.at("min_decision_margin").get<double>();

  // USB Camera list.
  settings.usb_camera_props = json.at("usb_cameras").get<std::vector<USBCameraProps>>();

  // MJPG Stream list.
  settings.mjpg_camera_props = json.at("mjpg_cameras").get<std::vector<MJPGCameraProps>>();
}