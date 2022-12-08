#pragma once

#include <ThunderCameraCalibrator/chessboard.h>
#include <opencv2/core/core.hpp>
#include <frc/geometry/Pose3d.h>
#include <vector>
#include <filesystem>

class Calibrator {
public:
  Calibrator(std::vector<std::filesystem::path> images, Chessboard chessboard);

  inline const Chessboard& get_chessboard() const { return chessboard; }

  /**
   * @brief Returns the matrix representing the camera's intrinsic properties.
   * 
   * @return The camera's intrinsic matrix.
   */
  inline const cv::Mat& get_camera_matrix() const { return camera_matrix; }

  /**
   * @brief Returns the matrix representing the camera's distortion coefficients.
   * 
   * @return The camera's distortion coefficients.
   */
  inline const cv::Mat& get_distortion_coefficients() const { return dist_coeffs; }

private:
  std::vector<std::filesystem::path> images;
  Chessboard chessboard;

  std::vector<std::vector<cv::Point3f>> object_points;
  std::vector<std::vector<cv::Point2f>> image_points;
  std::vector<cv::Point2f> corners;

  cv::Mat camera_matrix; // K
  cv::Mat dist_coeffs; // D
};