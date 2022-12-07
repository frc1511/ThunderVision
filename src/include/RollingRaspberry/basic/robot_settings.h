#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/config/config.h>
#include <RollingRaspberry/vision/vision_settings.h>

class RobotSettings {
public:
  RobotSettings();
  ~RobotSettings();
  
  inline VisionSettings get_vision_settings() { return vision_settings; }
  
private:
  wpi::json load_json(const std::filesystem::path& path);
  
  void load_vision_json();
  void load_field_json();
  
  VisionSettings vision_settings;
};
