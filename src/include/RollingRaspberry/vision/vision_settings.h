#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera_props.h>
#include <units/length.h>
#include <frc/apriltag/AprilTagFieldLayout.h>

struct VisionSettings {
  // Family of AprilTags to detect.
  std::string tag_family = "tag16h5";

  // Size of the AprilTags in meters.
  units::meter_t tag_size = 6_in;

  // Name of the field layout to use.
  std::string field_layout_name = "";
  frc::AprilTagFieldLayout* field_layout = nullptr;

  // The minimum decision margin for the AprilTag detector.
  double min_decision_margin = 100.0;

  // The number of iterations to run the pose estimator for.
  int estimate_iters = 50;

  // The maximum usable frame delay in seconds.
  double max_frame_delay = 0.25;

  std::vector<USBCameraProps> usb_camera_props;
  std::vector<MJPGCameraProps> mjpg_camera_props;
};

void to_json(wpi::json& json, const VisionSettings& settings);
void from_json(const wpi::json& json, VisionSettings& settings);
