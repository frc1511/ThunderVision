#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera_props.h>
#include <cameraserver/CameraServer.h>
#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>
#include <cscore_oo.h>

class CameraStream {
public:
  CameraStream(CameraProps props);
  virtual ~CameraStream();
  
  inline const CameraProps& get_props() const { return props; }
  
  inline std::uint64_t get_frame(cv::Mat& frame) { return cv_sink.GrabFrame(frame); }
  
  inline virtual cs::CvSource* get_ouput_source() { return nullptr; }
  
  inline std::string get_error() const { return cv_sink.GetError(); }
  std::string get_name() const;
  
  inline cs::CvSink& get_sink() { return cv_sink; }

  inline virtual bool should_run_detection() { return false; }
  inline virtual bool should_record() { return false; }
  
protected:
  void init(cs::VideoCamera* cam);

  CameraProps props;
  cs::VideoCamera* cam = nullptr;
  cs::CvSink cv_sink;
};

class USBCameraStream : public CameraStream {
public:
  /**
   * @brief USB Camera Stream.
   *
   * @param props The properties of the camera.
   */
  USBCameraStream(USBCameraProps props);
  ~USBCameraStream();
  
  /**
   * @brief Starts hosting the USB Camera Stream.
   *
   * The port the USB Camera Stream is hosted on counts up from 1180 to 1190.
   */
  void host_stream();

  inline bool is_hosting_stream() const { return hosting; }
  
  inline cs::CvSource* get_ouput_source() override { return hosting ? &output_source : nullptr; }

  inline bool should_run_detection() override { return running_detection; }

  inline bool should_record() override { return recording; }
  
private:
  cs::UsbCamera usb_camera;
  
  int dev;
  
  bool hosting = false;
  bool running_detection = false;
  bool recording = false;
  cs::CvSource output_source;
};

class MJPGCameraStream : public CameraStream {
public:
  /**
   * @brief MJPG Camera Stream.
   *
   * @param props The properties of the camera.
   */
  MJPGCameraStream(MJPGCameraProps props);
  ~MJPGCameraStream();

  inline bool should_run_detection() override { return running_detection; }

  inline bool should_record() override { return recording; }
  
private:
  cs::HttpCamera http_camera;
  
  std::string url;
  bool running_detection = false;
  bool recording = false;
};
