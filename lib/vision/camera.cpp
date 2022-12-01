#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/rolling_raspberry.h>

void to_json(wpi::json& json, const CameraProps& props) {
  json = wpi::json {
    { "name",     props.name },
    { "width",    props.width },
    { "height",   props.height },
    { "fps",      props.fps },
    { "fov",      props.fov.value() },
    { "position", props.pose.Translation() },
    { "rotation", wpi::json {
                    { "roll", props.pose.Rotation().X().value() },
                    { "pitch", props.pose.Rotation().Y().value() },
                    { "yaw", props.pose.Rotation().Z().value() }
                  }
    }
  };
}

void from_json(const wpi::json& json, CameraProps& props) {
  props.name   = json.at("name").get<std::string>();
  props.width  = json.at("width").get<std::size_t>();
  props.height = json.at("height").get<std::size_t>();
  props.fps    = json.at("fps").get<std::size_t>();
  props.fov    = units::degree_t(json.at("fov").get<double>());
  frc::Translation3d pos = json.at("position").get<frc::Translation3d>();

  wpi::json rot_json = json.at("rotation");

  units::radian_t roll = units::radian_t(rot_json.at("roll").get<double>());
  units::radian_t pitch = units::radian_t(rot_json.at("pitch").get<double>());
  units::radian_t yaw = units::radian_t(rot_json.at("yaw").get<double>());

  frc::Rotation3d rot(roll, pitch, yaw);

  props.pose = frc::Pose3d(pos, rot);
}

// --- CameraStream ---

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

const CameraProps& CameraStream::get_props() {
  return props;
}

std::uint64_t CameraStream::get_frame(cv::Mat frame) {
  return cv_sink.GrabFrame(frame);
}

// --- USBCameraStream ---

USBCameraStream::USBCameraStream(int _dev, CameraProps _props)
  : usb_camera(frc::CameraServer::StartAutomaticCapture(_props.name, [&]() -> int {
      return _dev;
    } ())), dev(_dev),
  CameraStream(_props, usb_camera) {
}

USBCameraStream::~USBCameraStream() = default;

cs::VideoSource& USBCameraStream::get_video_source() {
  return usb_camera;
}

void USBCameraStream::host_stream() {
  if (output_source) return;
  
  output_source = frc::CameraServer::PutVideo(props.name, props.width, props.height);
}

// --- MJPGCameraStream ---

MJPGCameraStream::MJPGCameraStream(std::string _url, CameraProps _props)
: http_camera(_props.name, _url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer),
  url(_url),
  CameraStream(_props, http_camera) {
}

MJPGCameraStream::~MJPGCameraStream() = default;

cs::VideoSource& MJPGCameraStream::get_video_source() {
  return http_camera;
}

