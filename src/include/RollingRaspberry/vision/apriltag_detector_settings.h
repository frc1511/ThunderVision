#pragma once

#include <RollingRaspberry/rolling_raspberry.h>

struct AprilTagDetectorSettings {
  float min_decision_margin = 100.0f;
};

void to_json(wpi::json& json, const AprilTagDetectorSettings& settings);
void from_json(const wpi::json& json, AprilTagDetectorSettings& settings);
