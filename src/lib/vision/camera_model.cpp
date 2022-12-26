#include <RollingRaspberry/vision/camera_model.h>

void to_json(wpi::json& json, const CameraModel& model) {
  json = wpi::json{
    {"intrinsic_matrix", model.matrix},
    {"distortion_coefficients", model.dist_coeffs}
  };
}

void from_json(const wpi::json& json, CameraModel& model) {
  model.matrix = json.at("intrinsic_matrix").get<std::array<double, 9>>();
  model.dist_coeffs = json.at("distortion_coefficients").get<std::array<double, 5>>();
}
