#include <RollingRaspberry/vision/vision_settings.h>

void to_json(wpi::json& json, const VisionSettings& settings) {
  json = wpi::json{
    {"camera_models", settings.camera_models},
    {"usb_cameras", settings.usb_camera_props},
    {"mjpg_cameras", settings.mjpg_camera_props},
    {"apriltag_detector", settings.apriltag_detector_settings}
  };
}

void from_json(const wpi::json& json, VisionSettings& settings) {
  // Camera Model list.
  settings.camera_models = json.at("camera_models").get<std::map<std::string, CameraModel>>();

  // USB Camera list.
  settings.usb_camera_props = json.at("usb_cameras").get<std::vector<USBCameraProps>>();

  // MJPG Stream list.
  settings.mjpg_camera_props = json.at("mjpg_cameras").get<std::vector<MJPGCameraProps>>();

  auto set_model = [&settings](CameraProps& props) {
    if (!settings.camera_models.count(props.model_name)) {
      throw std::runtime_error(fmt::format("Camera model '{}' not found.", props.model_name));
    }
    props.model = &settings.camera_models.at(props.model_name);
  };

  for (auto& camera : settings.usb_camera_props) {
    set_model(camera.props);
  }
  for (auto& camera : settings.mjpg_camera_props) {
    set_model(camera.props);
  }

  // AprilTag Detector Settings.
  settings.apriltag_detector_settings = json.at("apriltag_detector").get<AprilTagDetectorSettings>();
}