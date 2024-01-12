#pragma once

#include <wpi/json.h>

#include <array>
#include <filesystem>

struct CameraModel {
  std::size_t width;
  std::size_t height;

  //
  // Intrinsic properties of the camera (3x3 matrix).
  //
  // [ fx 0 cx ]
  // [ 0 fy cy ]
  // [ 0  0  1 ]
  //
  // fx, fy: focal length
  // cx, cy: principal point
  //
  std::array<double, 9> intrinsics;

  //
  // Extrinsic properties of the camera (1x5 matrix).
  //
  std::array<double, 5> distortion_coefficients;

  [[nodiscard]] static CameraModel
  load(const std::filesystem::path& config_file);
};
  
