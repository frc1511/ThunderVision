#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <Eigen/Core>

struct CameraModel {
  /**
   * Intrinsic matrix of the camera.
   * 
   * [ fx  0  cx ]
   * [  0  fy cy ]
   * [  0  0   1 ]
   * 
   * fx, fy: focal length
   * cx, cy: principal point
   */
  std::array<double, 9> matrix;

  // Distortion coefficients of the camera.
  std::array<double, 5> dist_coeffs;

  inline double get_fx() const { return matrix[0]; }
  inline double get_fy() const { return matrix[4]; }
  inline double get_cx() const { return matrix[2]; }
  inline double get_cy() const { return matrix[5]; }
};

void to_json(wpi::json& json, const CameraModel& model);
void from_json(const wpi::json& json, CameraModel& model);
