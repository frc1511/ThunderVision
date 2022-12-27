#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/config/config.h>
#include <RollingRaspberry/vision/vision_settings.h>
#include <RollingRaspberry/vision/camera_model.h>
#include <frc/apriltag/AprilTagFieldLayout.h>

class RobotSettings {
public:
  RobotSettings();
  ~RobotSettings();
  
  inline VisionSettings get_vision_settings() { return vision_settings; }
  
private:
  wpi::json load_json(const std::filesystem::path& path);
  
  void load_field_layouts_json();
  void load_camera_properties_json();
  void load_vision_settings_json();
  
  VisionSettings vision_settings;
  std::map<std::string, CameraModel> camera_models;
  std::map<std::string, frc::AprilTagFieldLayout> field_layouts;
};
