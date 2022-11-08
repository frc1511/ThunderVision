#pragma once

#include <cameraserver/CameraServer.h>
#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>
#include <cscore_oo.h>

class CameraStream {
public:
  // Properties of the camera.
  struct Props {
    std::string name = "the_camera";
    std::size_t width = 320, height = 240;
    std::size_t fps = 30;
  };

  CameraStream(Props props, cs::VideoSource& vid_src);
  virtual ~CameraStream();

  void set_props(Props props);
  Props get_props();

  virtual cv::Mat get_frame() = 0;

protected:
  virtual cs::VideoSource& get_video_source() = 0;

  Props props;
  cs::CvSink cv_sink;
};

class USBCameraStream : public CameraStream {
public:
  /**
   * @brief USB Camera Stream.
   *
   * @param dev The file descriptor id of the connected USB Camera.
   * @param props The properties of the camera stream.
   */
  USBCameraStream(int dev, Props props);
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
   * @param props The properties of the camera stream.
   */
  MJPGCameraStream(std::string url, Props props);
  ~MJPGCameraStream();
  
  cv::Mat get_frame() override;
  
  cs::VideoSource& get_video_source() override;
  
private:
  cs::HttpCamera http_camera;
};
