#include <RollingRaspberry/vision/apriltag_detector_settings.h>

void to_json(wpi::json& json, const AprilTagDetectorSettings& settings) {
  json = wpi::json {
    { "min_decision_margin", settings.min_decision_margin }
  };
}

void from_json(const wpi::json& json, AprilTagDetectorSettings& settings) {
  settings.min_decision_margin = json.at("min_decision_margin").get<float>();
}