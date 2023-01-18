#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <Eigen/Core>

struct CameraModel {
  std::size_t width = 1280;
  std::size_t height = 720;

  /**
   * Intrinsic matrix of the camera.
   * 
   * [ fx  0  cx ]
   * [  0  fy cy ]
   * [  0  0   1 ]
   * 
   * fx, fy: focal length
   * cx, cy: principal point
   * 
   * Values are in pixels.
   */
  std::array<double, 9> intrinsic_matrix;

  // Extrinsic matrix of the camera (distortion coefficients).
  std::array<double, 5> extrinsic_matrix;

  /**
   * @brief Returns the intrinsic camera matrix adjusted for the frame size.
   * 
   * @param width Width of the frame.
   * @param height Height of the frame.
   * 
   * @return The intrinsic camera matrix adjusted for the frame size.
   */
  std::array<double, 9> get_adjusted_intrinsic_matrix(std::size_t width, std::size_t height) const;
};

void to_json(wpi::json& json, const CameraModel& model);
void from_json(const wpi::json& json, CameraModel& model);
