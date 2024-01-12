#pragma once

#include <wpi/json.h>
#include <frc/geometry/Transform3d.h>

#include <cstddef>
#include <filesystem>
#include <string>

enum class CameraStreamType {
  USB,
  HTTP,
};

struct VisionModuleConfig {
  std::string name;
  frc::Transform3d robot_to_camera;

  std::size_t fps;
  bool detect_apriltags;
  bool record;
  std::filesystem::path record_path;

  std::string camera_model = "";

  CameraStreamType type;

  // union {

  struct {
    std::size_t device_id;
    bool host;
  } usb;

  struct {
    std::string url;
  } http;

  // };
};

void from_json(const wpi::json& j, VisionModuleConfig& config);

