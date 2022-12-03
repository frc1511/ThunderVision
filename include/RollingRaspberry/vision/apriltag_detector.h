#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/apriltag_detector_settings.h>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <apriltag.h>
#include <tag16h5.h>

class AprilTagDetector {
public:
  AprilTagDetector(AprilTagDetectorSettings settings);
  ~AprilTagDetector();
  
  struct Detection {
    int id;
    int hamming;
    float decision_margin;
    Eigen::Matrix<double, 3, 3> homography;
    
    cv::Point2d center;
    std::array<cv::Point2d, 4> corners;
  };
  
  /**
   * @brief Detects AprilTags in the given frame.
   *
   * @param frame The frame to detect tags in (must be grayscale).
   */
  std::vector<Detection> detect(cv::Mat& frame);
  
  /**
   * @brief Visualizes the given detection on the given frame.
   *
   * @param frame The frame to visualize the detection on.
   * @param detection The AprilTag detection to visualize.
   */
  void visualize_detection(cv::Mat& frame, Detection& detection);
  
private:
  AprilTagDetectorSettings settings;
  
  apriltag_family_t* tag_family;
  apriltag_detector_t* tag_detector;
};
