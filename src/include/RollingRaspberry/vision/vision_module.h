#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/vision/vision_settings.h>
#include <frc/apriltag/AprilTagDetector.h>
#include <frc/apriltag/AprilTagDetection.h>
#include <frc/apriltag/AprilTagPoseEstimator.h>
#include <frc/apriltag/AprilTagPoseEstimate.h>
#include <frc/geometry/Transform3d.h>
#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>

class VisionModule {
public:
  VisionModule(CameraStream* stream, const VisionSettings* settings);
  ~VisionModule();
  
  /**
   * @brief Initializes the vision module thread.
   */
  void init_thread();
  
  /**
   * @brief Sets whether the Vision Module thread is running.
   *
   * @param running Whether the Vision Module thread is running.
   */
  void set_running(bool running);

  /**
   * @brief Returns whether the Vision Module thread is running.
   *
   * @return Whether the Vision Module thread is running.
   */
  bool is_running() const;
  
  /**
   * @brief Terminates the Vision Module thread.
   */
  void terminate();

  /**
   * @brief Returns whether the Vision Module thread has terminated.
   *
   * @return Whether the Vision Module thread has terminated.
   */
  bool is_terminated() const;

  /**
   * @brief Returns the time point of the last pose estimates.
   * 
   * @return The time point of the last pose estimates.
   */
  std::chrono::high_resolution_clock::time_point get_pose_estimates_time_point() const;

  /**
   * @brief Returns whether the module has new tag pose estimates.
   * 
   * @return Whether the module has new tag pose estimates.
   */
  bool has_new_pose_estimates() const;

  /**
   * @brief Returns the last tag pose estimates. This function will reset the 'new pose estimate' status.
   * 
   * @return The last tag pose estimates.
   */
  std::vector<frc::AprilTagPoseEstimate> get_pose_estimates();

  /**
   * @brief Returns the transform from the robot center to the camera.
   * 
   * @return The transform from the robot center to the camera.
   */
  frc::Transform3d get_robot_to_camera_transform() const;
  
private:
  /**
   * @brief Starts the Vision Module thread.
   */
  void thread_start();

  /**
   * @brief Visualizes a AprilTag detection.
   * 
   * @param frame The frame to draw the detection on.
   * @param detection The AprilTag detection to draw.
   */
  static void visualize_detection(cv::Mat& frame, const frc::AprilTagDetection& detection);

  const VisionSettings* settings;
  
  CameraStream* cam_stream;
  const CameraProps* cam_props;
  const frc::Transform3d robot_to_camera;
  const std::array<double, 9> cam_matrix;
  
  frc::AprilTagDetector tag_detector;

  std::chrono::high_resolution_clock::time_point pose_estimates_time_point;
  std::vector<frc::AprilTagPoseEstimate> pose_estimates;
  bool new_pose_estimates = false;

  frc::AprilTagPoseEstimator pose_estimator;
  
  bool thread_running = false;
  bool thread_terminated = true;
  
  std::thread module_thread;
  mutable std::mutex module_mutex;
};
