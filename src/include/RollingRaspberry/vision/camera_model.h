#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <Eigen/Core>

struct CameraModel {
  // Intrinsic matrix of the camera.
  std::array<double, 9> matrix;

  // Distortion coefficients of the camera.
  std::array<double, 5> dist_coeffs;
};

void to_json(wpi::json& json, const CameraModel& model);
void from_json(const wpi::json& json, CameraModel& model);
