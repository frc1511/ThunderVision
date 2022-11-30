#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <cameraserver/CameraServer.h>
#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>
#include <cscore_oo.h>
#include <frc/geometry/Pose3d.h>

struct CameraProps {
  // The identifier string of the camera.
  std::string name = "the_camera";
  
  // The pixel dimensions of the camera.
  std::size_t width = 320,
              height = 240;
  
  // The fps of the camera.
  std::size_t fps = 30;
  
  // Field of view of the camera.
  units::degree_t fov = 68.5_deg;
  
  frc::Pose3d pose;
};

void to_json(wpi::json& json, const CameraProps& props);
void from_json(const wpi::json& json, CameraProps& props);

class CameraStream {
public:
  CameraStream(CameraProps props, cs::VideoSource& vid_src);
  virtual ~CameraStream();
  
  void set_props(CameraProps props);
  CameraProps get_props();
  
  virtual cv::Mat get_frame() = 0;
  
protected:
  virtual cs::VideoSource& get_video_source() = 0;
  
  CameraProps props;
  cs::CvSink cv_sink;
};

class USBCameraStream : public CameraStream {
public:
  /**
   * @brief USB Camera Stream.
   *
   * @param dev The file descriptor id of the connected USB Camera.
   * @param props The properties of the camera.
   */
  USBCameraStream(int dev, CameraProps props);
  ~USBCameraStream();
  
  cv::Mat get_frame() override;
  
  cs::VideoSource& get_video_source() override;
  
  /**
   * @brief Starts hosting the USB Camera Stream.
   *
   * The port the USB Camera Stream is hosted on counts up from 1180 to 1190.
   */
  void host_stream();
  
private:
  cs::UsbCamera usb_camera;
  
  bool hosting_stream = false;
};

class MJPGCameraStream : public CameraStream {
public:
  /**
   * @brief MJPG Camera Stream.
   *
   * @param url The URL of the MJPG Camera Stream.
   * @param props The properties of the camera.
   */
  MJPGCameraStream(std::string url, CameraProps props);
  ~MJPGCameraStream();
  
  cv::Mat get_frame() override;
  
  cs::VideoSource& get_video_source() override;
  
private:
  cs::HttpCamera http_camera;
};
