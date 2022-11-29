#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/rolling_raspberry.h>

void to_json(wpi::json& json, const CameraProps& props) {
  json = wpi::json {
    { "name",     props.name },
    { "width",    props.width },
    { "height",   props.height },
    { "fps",      props.fps },
    { "position", props.pos }
  };
}

void from_json(const wpi::json& json, CameraProps& props) {
  props.name   = json.at("name").get<std::string>();
  props.width  = json.at("width").get<std::size_t>();
  props.height = json.at("height").get<std::size_t>();
  props.fps    = json.at("fps").get<std::size_t>();
  props.pos    = json.at("position").get<frc::Translation3d>();
}

CameraStream::CameraStream(CameraProps _props, cs::VideoSource& vid_src)
: props(_props), cv_sink(props.name) {
  vid_src.SetResolution(props.width, props.height);
  vid_src.SetFPS(props.fps);

  cv_sink.SetSource(vid_src);
  cv_sink.SetEnabled(true);
}

CameraStream::~CameraStream() = default;

void CameraStream::set_props(CameraProps _props) {
  props = _props;
}

CameraProps CameraStream::get_props() {
  return props;
}

USBCameraStream::USBCameraStream(int dev, CameraProps _props)
  : usb_camera(frc::CameraServer::StartAutomaticCapture(_props.name, [&]() -> int {
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

MJPGCameraStream::MJPGCameraStream(std::string url, CameraProps _props)
: http_camera(_props.name, url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer),
  CameraStream(_props, http_camera) {
}

MJPGCameraStream::~MJPGCameraStream() = default;

cv::Mat MJPGCameraStream::get_frame() {
  
}

cs::VideoSource& MJPGCameraStream::get_video_source() {
  return http_camera;
}

