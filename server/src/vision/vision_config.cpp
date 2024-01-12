#include <thunder_vision/vision/vision_config.hpp>

#include <thunder_vision/diagnostics.hpp>

#include <wpi/json.h>
#include <wpi/raw_istream.h>

#include <cstdio>
#include <set>

#define CONFIG_FILE_NAME "vision.json"

static void from_json(const wpi::json& j, VisionConfig& config) {
  // clang-format off
  config.tag_family               = j.at("apriltag_family").get<std::string>();
  config.tag_size                 = units::meter_t(j.at("apriltag_size").get<double>());
  config.field_layout_name        = j.at("field_layout").get<std::string>();
  config.min_decision_margin      = j.at("min_decision_margin").get<double>();
  config.pose_estimate_iterations = j.at("pose_estimate_iterations").get<uint32_t>();
  config.max_frame_delay          = j.at("max_frame_delay").get<double>();
  config.max_elevation            = units::meter_t(j.at("max_elevation").get<double>());
  config.pose_tolerance           = units::meter_t(j.at("pose_tolerance").get<double>());
  config.modules                  = j.at("vision_modules").get<std::vector<VisionModuleConfig>>();
  // clang-format on
}

VisionConfig VisionConfig::load(const std::filesystem::path& config_path) {
  const std::filesystem::path vision_config_file =
      config_path / CONFIG_FILE_NAME;

  std::error_code error;
  wpi::raw_fd_istream stream(vision_config_file.c_str(), error);

  if (error) {
    fprintf(stderr, ERROR_TAG "Failed to open file " CONFIG_FILE_NAME ": %s\n",
            error.message().c_str());
    exit(1);
  }

  VisionConfig config;
  try {
    wpi::json json;
    stream >> json;
    config = json.get<VisionConfig>();

  } catch (const wpi::json::exception& e) {
    fprintf(stderr, ERROR_TAG "Failed to parse file " CONFIG_FILE_NAME ": %s\n",
            e.what());
    exit(1);
  }

  std::set<std::string> camera_model_names;

  for (const VisionModuleConfig& module_config : config.modules) {
    camera_model_names.insert(module_config.camera_model);
  }

  for (const std::string& camera_model_name : camera_model_names) {
    const std::filesystem::path camera_model_file =
        config_path / (camera_model_name + ".json");

    config.camera_models[camera_model_name] =
        CameraModel::load(camera_model_file);
  }

  return config;
};

