#pragma once

#include <thunder_vision/networking/networking_config.hpp>
#include <thunder_vision/vision/vision_config.hpp>

#include <frc/apriltag/AprilTagFieldLayout.h>

#include <cstdint>
#include <filesystem>

struct Config {
  std::filesystem::path config_path;

  NetworkingConfig networking_config;
  VisionConfig vision_config;
  frc::AprilTagFieldLayout field_layout;

  [[nodiscard]] static Config load(uint32_t argc, const char** argv);

private:
  void check_environment_variables();
  void check_arguments(uint32_t argc, const char** argv);
};

