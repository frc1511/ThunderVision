#pragma once

#include <RollingRaspberry/config/settings.h>
#include <RollingRaspberry/basic/subsystem.h>
#include <RollingRaspberry/vision/camera.h>

#include <apriltag.h>
#include <tag16h5.h>

#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>

class VisionModule {
public:
  VisionModule(CameraStream* stream);
  ~VisionModule();
  
  VisionModule(VisionModule&& other);
  
  /**
   * @brief Sets whether the Vision Module thread is running.
   *
   * @param running Whether the Vision Module thread is running.
   */
  void set_running(bool running);
  
  /**
   * @brief Terminates the Vision Module thread.
   */
  void terminate();
  
private:
  void thread_start();

  CameraStream* cam_stream;

  bool thread_running = true;
  bool thread_terminated = false;
  
  std::thread module_thread;
  std::mutex module_mutex;
};

class Vision : public Subsystem {
public:
  Vision(Settings& settings);
  ~Vision();
  
  void reset_to_mode(NTHandler::MatchMode mode) override;
  void process() override;
  
private:
  std::vector<USBCameraStream>& usb_cameras;
  std::vector<MJPGCameraStream>& mjpg_cameras;
  
  std::vector<VisionModule> vision_modules;
};
