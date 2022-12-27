#include <RollingRaspberry/basic/robot_settings.h>
#include <wpi/raw_istream.h>

RobotSettings::RobotSettings() {
  load_camera_properties_json();
  load_field_layouts_json();
  load_vision_settings_json();

  // --- Resolve field layout references ---

  if (!field_layouts.count(vision_settings.field_layout_name)) {
    throw std::runtime_error(fmt::format("Failed to find field layout with name {}", vision_settings.field_layout_name));
  }
  vision_settings.field_layout = &field_layouts[vision_settings.field_layout_name];

  // --- Resolve camera model references ---

  auto find_camera_model = [&](const CameraProps& props) -> CameraModel* {
    if (!camera_models.count(props.model_name)) {
      throw std::runtime_error(fmt::format("Failed to find camera model with name {}", props.model_name));
    }

    return &camera_models[props.model_name];
  };

  for (auto& usb_camera : vision_settings.usb_camera_props) {
    usb_camera.props.model = find_camera_model(usb_camera.props);
  }
  for (auto& mjpg_camera : vision_settings.mjpg_camera_props) {
    mjpg_camera.props.model = find_camera_model(mjpg_camera.props);
  }

  fmt::print(FRC1511_LOG_PREFIX "Successfully loaded all configuration json files.\n");
}

RobotSettings::~RobotSettings() = default;

wpi::json RobotSettings::load_json(const std::filesystem::path& path) {
  std::error_code err_code;

  const char* path_str = path.c_str();
  
  wpi::raw_fd_istream cfg_file { path_str, err_code };
  if (err_code) {
    fmt::print(FRC1511_LOG_PREFIX "Failed to open configuration json file at {}\n", path_str);
    // TODO: Create error handling system to exit program cleanly.
    return {};
  }
  
  wpi::json cfg_json;
  cfg_file >> cfg_json;
  
  return cfg_json;
}

void RobotSettings::load_field_layouts_json() {
  wpi::json json = load_json(THUNDER_CONFIG_FIELD_LAYOUTS_JSON);

  field_layouts = json.get<std::map<std::string, frc::AprilTagFieldLayout>>();

  fmt::print(FRC1511_LOG_PREFIX "Successfully loaded configuration json file at {}\n", THUNDER_CONFIG_FIELD_LAYOUTS_JSON.string());
}

void RobotSettings::load_camera_properties_json() {
  wpi::json json = load_json(THUNDER_CONFIG_CAMERA_PROPERTIES_JSON);

  camera_models = json.get<std::map<std::string, CameraModel>>();

  fmt::print(FRC1511_LOG_PREFIX "Successfully loaded configuration json file at {}\n", THUNDER_CONFIG_CAMERA_PROPERTIES_JSON.string());
}

void RobotSettings::load_vision_settings_json() {
  wpi::json json = load_json(THUNDER_CONFIG_VISION_SETTINGS_JSON);

  vision_settings = json.get<VisionSettings>();

  fmt::print(FRC1511_LOG_PREFIX "Successfully loaded configuration json file at {}\n", THUNDER_CONFIG_VISION_SETTINGS_JSON.string());
}
