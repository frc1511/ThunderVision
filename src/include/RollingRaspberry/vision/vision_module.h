#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/vision/apriltag_detector_settings.h>
#include <frc/apriltag/AprilTagDetector.h>
#include <frc/apriltag/AprilTagDetection.h>
#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>

class VisionModule {
public:
  VisionModule(CameraStream* stream, AprilTagDetectorSettings settings);
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
  
  CameraStream* cam_stream;

  AprilTagDetectorSettings settings;
  
  frc::AprilTagDetector tag_detector;
  
  bool thread_running = false;
  bool thread_terminated = true;
  
  std::thread module_thread;
  mutable std::mutex module_mutex;
};
