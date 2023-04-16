#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/rolling_raspberry.h>

// --- CameraStream ---

CameraStream::CameraStream(CameraProps _props)
: props(_props), cv_sink(_props.name) { }

CameraStream::~CameraStream() = default;

void CameraStream::init(cs::VideoCamera* _cam) {
  cam = _cam;
  cam->SetResolution(props.width, props.height);
  cam->SetFPS(props.fps);

  cv_sink.SetSource(*cam);
  cv_sink.SetEnabled(true);
}

std::string CameraStream::get_name() const {
  return props.name;
}

// --- USBCameraStream ---

USBCameraStream::USBCameraStream(USBCameraProps _props)
: CameraStream(_props.props), usb_camera(props.name, _props.dev), dev(_props.dev), running_detection(_props.do_detection) {
  init(&usb_camera);

  if (_props.host_stream) {
    host_stream();
  }
}

USBCameraStream::~USBCameraStream() = default;

void USBCameraStream::host_stream() {
  if (hosting) return;
  hosting = true;
  
  output_source = frc::CameraServer::PutVideo(props.name, props.width, props.height);
}

// --- MJPGCameraStream ---

MJPGCameraStream::MJPGCameraStream(MJPGCameraProps _props)
: CameraStream(_props.props), http_camera(props.name, _props.url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer), url(_props.url), running_detection(_props.do_detection) {
  init(&http_camera);
}

MJPGCameraStream::~MJPGCameraStream() = default;
