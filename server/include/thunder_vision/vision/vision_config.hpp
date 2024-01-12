#pragma once

#include <thunder_vision/vision/camera_model.hpp>
#include <thunder_vision/vision/vision_module_config.hpp>

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <units/length.h>
#include <vector>

struct VisionConfig {
  std::string tag_family;
  units::meter_t tag_size;
  std::string field_layout_name;

  double min_decision_margin;
  uint32_t pose_estimate_iterations;
  double max_frame_delay;

  units::meter_t max_elevation;
  units::meter_t pose_tolerance;

  std::vector<VisionModuleConfig> modules;

  std::map<std::string, CameraModel> camera_models;

  [[nodiscard]] static VisionConfig
  load(const std::filesystem::path& config_path);
};

