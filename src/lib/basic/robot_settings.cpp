#include <RollingRaspberry/basic/robot_settings.h>
#include <wpi/raw_istream.h>

RobotSettings::RobotSettings() {
  load_vision_json();
  load_field_json();
}

RobotSettings::~RobotSettings() = default;

wpi::json RobotSettings::load_json(const std::filesystem::path& path) {
  std::error_code err_code;

  const char* path_str = path.c_str();
  
  wpi::raw_fd_istream cfg_file { path_str, err_code };
  if (err_code) {
    fmt::print(FRC1511_LOG_PREFIX "Failed to load configuration json file at {}\n", path_str);
    // TODO: Create error handling system to exit program cleanly.
    return {};
  }

  fmt::print(FRC1511_LOG_PREFIX "Successfully loaded configuration json file at {}\n", path_str);
  
  wpi::json cfg_json;
  cfg_file >> cfg_json;
  
  return cfg_json;
}

void RobotSettings::load_vision_json() {
  wpi::json json = load_json(THUNDER_VISION_CONFIG_JSON);

  vision_settings = json.get<VisionSettings>();
}

void RobotSettings::load_field_json() {
  wpi::json json = load_json(THUNDER_FIELD_CONFIG_JSON);
}
