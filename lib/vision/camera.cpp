#include <ThunderRPi/vision/camera.h>
#include <ThunderRPi/thunder_rpi.h>

CameraStream::CameraStream(Props _props, cs::VideoSource& vid_src)
: props(_props), cv_sink(props.name) {
  std::cout << "hi\n";
  vid_src.SetResolution(props.width, props.height);
  vid_src.SetFPS(props.fps);

  cv_sink.SetSource(vid_src);
  cv_sink.SetEnabled(true);
}

CameraStream::~CameraStream() = default;

void CameraStream::set_props(CameraStream::Props _props) {
  props = _props;
}

CameraStream::Props CameraStream::get_props() {
  return props;
}

USBCameraStream::USBCameraStream(int dev, CameraStream::Props _props)
  : usb_camera(frc::CameraServer::StartAutomaticCapture(_props.name, [&]() -> int {
        std::cout << "gtrgtrg\n";
        return dev;
        } ())),
  CameraStream(_props, usb_camera) {
}

USBCameraStream::~USBCameraStream() = default;

cv::Mat USBCameraStream::get_frame() {
  
}

cs::VideoSource& USBCameraStream::get_video_source() {
  return usb_camera;
}

void USBCameraStream::host_stream() {
  if (hosting_stream) return;
  hosting_stream = true;

  frc::CameraServer::PutVideo(props.name, props.width, props.height);
}


MJPGCameraStream::MJPGCameraStream(std::string url, CameraStream::Props _props)
: http_camera(_props.name, url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer),
  CameraStream(_props, http_camera) {
}

MJPGCameraStream::~MJPGCameraStream() = default;

cv::Mat MJPGCameraStream::get_frame() {
  
}

cs::VideoSource& MJPGCameraStream::get_video_source() {
  return http_camera;
}
