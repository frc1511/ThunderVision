#pragma once

#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include <frc/apriltag/AprilTagFieldLayout.h>
#include <frc/apriltag/AprilTagPoseEstimate.h>
#include <frc/geometry/Pose3d.h>

struct VisionConfig;
struct VisionModuleConfig;
class Networking;
class CameraStream;

class Vision {
  const VisionConfig& m_config;
  const frc::AprilTagFieldLayout& m_field_layout;
  Networking& m_networking;

  std::vector<std::thread> m_vision_modules;

  std::mutex m_terminated_mutex;
  bool m_terminated;

  std::mutex m_detection_mutex;

  struct Detection {
    int32_t tag_id;
    const VisionModuleConfig& module_config;
    frc::AprilTagPoseEstimate estimate;
  };

  std::vector<Detection> m_detections;

public:
  Vision(const VisionConfig& config,
         const frc::AprilTagFieldLayout& field_layout, Networking& networking);
  ~Vision();

  void process();

private:
  void setup_modules();

  std::optional<frc::Pose3d>
  calculate_robot_pose(const Detection& detection) const;

  bool validate_robot_pose(const frc::Pose3d& robot_pose) const;

  void vision_module(CameraStream* camera_stream,
                     const VisionModuleConfig& module_config);
};

