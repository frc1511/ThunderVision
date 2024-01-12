#include <thunder_vision/vision/vision_module_config.hpp>

#include <thunder_vision/diagnostics.hpp>

#include <frc/geometry/Pose3d.h>

void from_json(const wpi::json& j, VisionModuleConfig& config) {
  config.name = j.at("name").get<std::string>();

  const frc::Pose3d camera_pose = j.at("robot_to_camera").get<frc::Pose3d>();
  config.robot_to_camera =
      frc::Transform3d(camera_pose.Translation(), camera_pose.Rotation());

  config.fps = j.at("fps").get<std::size_t>();
  config.detect_apriltags = j.at("detect_apriltags").get<bool>();
  config.record = j.at("record").get<bool>();
  if (config.record) {
    config.record_path = j.at("record_path").get<std::string>();
  }

  config.camera_model = j.at("camera").get<std::string>();
  const std::string type = j.at("stream_type").get<std::string>();
  if (type == "usb") {
    config.type = CameraStreamType::USB;
    config.usb = {
        .device_id = j.at("device_id").get<std::size_t>(),
        .host = j.at("host").get<bool>(),
    };
  } else if (type == "http") {
    config.type = CameraStreamType::HTTP;
    config.http = {
        .url = j.at("url").get<std::string>(),
    };
  } else {
    fprintf(stderr, ERROR_TAG "Unknown stream type: %s\n", type.c_str());
    exit(1);
  }
}

