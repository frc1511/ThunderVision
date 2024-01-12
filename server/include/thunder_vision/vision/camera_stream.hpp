#pragma once

#include <cstdint>
#include <map>
#include <string>

#include <cscore.h>
#include <cscore_cv.h>
#include <cscore_oo.h>
#include <opencv2/opencv.hpp>
#include <cameraserver/CameraServer.h>

struct CameraModel;

class CameraStream {
protected:
  const char* m_name;
  const CameraModel& m_model;
  const uint32_t m_fps;
  const bool m_detect_apriltags;
  const bool m_record;

  cs::CvSink m_sink;

protected:
  CameraStream(const char* name, const CameraModel& model, uint32_t fps,
               bool detect_apriltags, bool record);

  void init_camera(cs::VideoCamera& camera);

public:
  virtual ~CameraStream() = default;

  uint64_t get_frame(cv::Mat& frame) { return m_sink.GrabFrame(frame); }
  std::string error() const { return m_sink.GetError(); }

  virtual cs::CvSource* output() { return nullptr; }

  const char* name() const { return m_name; }
  const CameraModel& model() const { return m_model; }
  uint32_t fps() const { return m_fps; }
  bool detect_apriltags() const { return m_detect_apriltags; }
  bool record() const { return m_record; }
};

struct VisionModuleConfig;

// Returns a camera stream for the given config (allocated on the heap).
[[nodiscard]] CameraStream*
load_camera_stream(const VisionModuleConfig& config,
                   const std::map<std::string, CameraModel>& models);

