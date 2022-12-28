#include <RollingRaspberry/vision/camera_model.h>

std::array<double, 9> CameraModel::get_adjusted_matrix(std::size_t new_width, std::size_t new_height) const {
  std::array<double, 9> adjusted_matrix = matrix;

  adjusted_matrix[0] *= new_width  / static_cast<double>(width);  // fx
  adjusted_matrix[4] *= new_height / static_cast<double>(height); // fy
  adjusted_matrix[2] *= new_width  / static_cast<double>(width);  // cx
  adjusted_matrix[5] *= new_height / static_cast<double>(height); // cy

  return adjusted_matrix;
}

void to_json(wpi::json& json, const CameraModel& model) {
  json = wpi::json{
    {"width", model.width},
    {"height", model.height},
    {"intrinsic_matrix", model.matrix},
    {"distortion_coefficients", model.dist_coeffs}
  };
}

void from_json(const wpi::json& json, CameraModel& model) {
  model.width = json.at("width").get<std::size_t>();
  model.height = json.at("height").get<std::size_t>();
  model.matrix = json.at("intrinsic_matrix").get<std::array<double, 9>>();
  model.dist_coeffs = json.at("distortion_coefficients").get<std::array<double, 5>>();
}
