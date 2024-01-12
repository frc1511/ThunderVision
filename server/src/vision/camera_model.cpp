#include <thunder_vision/vision/camera_model.hpp>

#include <thunder_vision/diagnostics.hpp>

#include <wpi/json.h>
#include <wpi/raw_istream.h>

#include <cstdio>

static void from_json(const wpi::json& j, CameraModel& config) {
  config.width = j.at("width").get<std::size_t>();
  config.height = j.at("height").get<std::size_t>();
  config.intrinsics = j.at("intrinsics").get<std::array<double, 9>>();
  config.distortion_coefficients =
      j.at("distortion_coefficients").get<std::array<double, 5>>();
}

CameraModel CameraModel::load(const std::filesystem::path& config_file) {
  std::error_code error;
  wpi::raw_fd_istream stream(config_file.c_str(), error);

  const char* filename = config_file.filename().c_str();

  if (error) {
    fprintf(stderr, ERROR_TAG "Failed to open file %s: %s\n", filename,
            error.message().c_str());
    exit(1);
  }

  CameraModel model;
  try {
    wpi::json json;
    stream >> json;
    model = json.get<CameraModel>();

  } catch (const wpi::json::exception& e) {
    fprintf(stderr, ERROR_TAG "Failed to parse file %s: %s\n", filename,
            e.what());
    exit(1);
  }

  return model;
}

