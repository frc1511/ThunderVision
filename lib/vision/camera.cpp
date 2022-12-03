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

CameraStream::CameraStream(CameraProps _props, cs::VideoCamera* _cam)
: props(_props), cam(_cam), cv_sink(_props.name) {
  cam->SetResolution(props.width, props.height);
  cam->SetFPS(props.fps);

  cv_sink.SetSource(*cam);
  cv_sink.SetEnabled(true);
}

CameraStream::~CameraStream() = default;

void CameraStream::set_props(CameraProps _props) {
  props = _props;
  // TODO: Apply everything
}

// --- USBCameraStream ---

USBCameraStream::USBCameraStream(int _dev, CameraProps _props)
: CameraStream(_props, [&]() -> cs::VideoCamera* {
    usb_camera = cs::UsbCamera(_props.name, _dev);
    return &usb_camera;
  }()) {
}

USBCameraStream::~USBCameraStream() = default;

void USBCameraStream::host_stream() {
  if (hosting_stream) return;
  
  hosting_stream = true;
  
  output_source = frc::CameraServer::PutVideo(props.name, props.width, props.height);
}

// --- MJPGCameraStream ---

MJPGCameraStream::MJPGCameraStream(std::string _url, CameraProps _props)
: http_camera(_props.name, _url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer),
  url(_url),
  CameraStream(_props, &http_camera) {
}

MJPGCameraStream::~MJPGCameraStream() = default;
