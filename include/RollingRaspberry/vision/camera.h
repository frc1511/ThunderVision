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
  CameraStream(CameraProps props, cs::VideoCamera* cam);
  virtual ~CameraStream();
  
  void set_props(CameraProps props);
  inline const CameraProps& get_props() const { return props; }
  
  inline std::uint64_t get_frame(cv::Mat& frame) { return cv_sink.GrabFrame(frame); }
  
  inline virtual cs::CvSource* get_ouput_source() { return nullptr; }
  
  inline std::string get_error() const { return cv_sink.GetError(); }
  inline std::string get_desc() const { return cam->GetDescription(); }

  inline cs::CvSink& get_sink() { return cv_sink; }
  
protected:
  CameraProps props;
  cs::VideoCamera* cam;
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
  
  /**
   * @brief Starts hosting the USB Camera Stream.
   *
   * The port the USB Camera Stream is hosted on counts up from 1180 to 1190.
   */
  void host_stream();
  
  inline cs::CvSource* get_ouput_source() override { return hosting_stream ? &output_source : nullptr; }
  
private:
  cs::UsbCamera usb_camera;
  
  int dev;
  
  bool hosting_stream = false;
  cs::CvSource output_source;
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
  
private:
  cs::HttpCamera http_camera;
  
  std::string url;
};
