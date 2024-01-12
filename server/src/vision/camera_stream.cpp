#include <thunder_vision/vision/camera_stream.hpp>

#include <thunder_vision/vision/camera_model.hpp>
#include <thunder_vision/vision/vision_module_config.hpp>
#include <thunder_vision/diagnostics.hpp>

#include <cstdio>

CameraStream::CameraStream(const char* name, const CameraModel& model,
                           uint32_t fps, bool detect_apriltags, bool record)
  : m_name(name),
    m_model(model),
    m_fps(fps),
    m_detect_apriltags(detect_apriltags),
    m_record(record),
    m_sink(m_name) {}

void CameraStream::init_camera(cs::VideoCamera& camera) {
  camera.SetResolution(m_model.width, m_model.height);
  camera.SetFPS(m_fps);

  m_sink.SetSource(camera);
  m_sink.SetEnabled(true);
}

class CameraStreamUSB : public CameraStream {
  cs::UsbCamera m_camera;
  cs::CvSource m_output;

public:
  CameraStreamUSB(const VisionModuleConfig& config, const CameraModel& model)
    : CameraStream(config.name.c_str(), model, config.fps,
                   config.detect_apriltags, config.record),
      m_camera(name(), config.usb.device_id) {

    init_camera(m_camera);

    if (config.usb.host) {
      printf(VISION_TAG "Hosting USB camera stream %s\n", name());
      m_output =
          frc::CameraServer::PutVideo(name(), m_model.width, m_model.height);
    }
  }

  cs::CvSource* output() override { return &m_output; }

  ~CameraStreamUSB() override = default;
};

class CameraStreamHTTP : public CameraStream {
  cs::HttpCamera m_camera;

public:
  CameraStreamHTTP(const VisionModuleConfig& config, const CameraModel& model)
    : CameraStream(config.name.c_str(), model, config.fps,
                   config.detect_apriltags, config.record),
      m_camera(name(), config.http.url) {

    init_camera(m_camera);
  }

  ~CameraStreamHTTP() override = default;
};

CameraStream*
load_camera_stream(const VisionModuleConfig& config,
                   const std::map<std::string, CameraModel>& models) {

  const CameraModel& model = models.at(config.camera_model);

  if (config.type == CameraStreamType::USB) {
    return new CameraStreamUSB(config, model);

  } else if (config.type == CameraStreamType::HTTP) {
    return new CameraStreamHTTP(config, model);
  }
  assert(false);
};

