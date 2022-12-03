#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera_props.h>
#include <RollingRaspberry/vision/apriltag_detector_settings.h>

struct VisionSettings {
  std::vector<USBCameraProps> usb_camera_props;
  std::vector<MJPGCameraProps> mjpg_camera_props;

  AprilTagDetectorSettings apriltag_detector_settings;
};

void to_json(wpi::json& json, const VisionSettings& settings);
void from_json(const wpi::json& json, VisionSettings& settings);
