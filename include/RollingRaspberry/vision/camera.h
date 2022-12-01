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
  
  enum class Type {
    USB,
    MJPG,
  };
  
  virtual Type get_type() = 0;
  
  virtual std::string get_desc() = 0;
  
  void set_props(CameraProps props);
  const CameraProps& get_props();
  
  virtual std::uint64_t get_frame(cv::Mat frame);
  
  virtual cs::VideoSource& get_video_source() = 0;
  
  inline virtual cs::CvSource* get_ouput_source() { return nullptr; }
  
protected:
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
  
  inline Type get_type() override { return Type::USB; }
  
  inline std::string get_desc() override { return fmt::format("USB Camera (ID: {})", dev); }
  
  cs::VideoSource& get_video_source() override;
  
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
  
  inline Type get_type() override { return Type::MJPG; }
  
  inline std::string get_desc() override { return fmt::format("MJPG Camera Stream (URL: {})", url); }
  
  cs::VideoSource& get_video_source() override;
  
private:
  cs::HttpCamera http_camera;
  
  std::string url;
};
