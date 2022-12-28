#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera_props.h>
#include <units/length.h>
#include <frc/apriltag/AprilTagFieldLayout.h>

struct VisionSettings {
  std::string tag_family = "tag16h5";
  units::meter_t tag_size = 6_in;
  std::string field_layout_name = "";
  frc::AprilTagFieldLayout* field_layout = nullptr;
  double min_decision_margin = 100.0;
  int estimate_iters = 50;

  std::vector<USBCameraProps> usb_camera_props;
  std::vector<MJPGCameraProps> mjpg_camera_props;
};

void to_json(wpi::json& json, const VisionSettings& settings);
void from_json(const wpi::json& json, VisionSettings& settings);
