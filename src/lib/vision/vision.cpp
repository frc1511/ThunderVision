#include <RollingRaspberry/vision/vision.h>
#include <RollingRaspberry/network/nt_handler.h>
#include <RollingRaspberry/basic/clock.h>

Vision::Vision() 
: vision_settings(robot_settings.get_vision_settings()) {

  for (const auto& props : vision_settings.usb_camera_props) {
    usb_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&usb_cameras.back(), &vision_settings));
  }
  for (const auto& props : vision_settings.mjpg_camera_props) {
    mjpg_cameras.emplace_back(props);
    vision_modules.push_back(std::make_unique<VisionModule>(&mjpg_cameras.back(), &vision_settings));
  }

  for (auto& mod : vision_modules) {
    mod->init_thread();
  }
}

Vision::~Vision() = default;

void Vision::reset_to_mode(NTHandler::MatchMode mode) { }

void Vision::process() {
  using PoseEstimation = std::tuple<units::second_t, int, frc::AprilTagPoseEstimate>;
  std::vector<std::pair<frc::Transform3d, PoseEstimation>> pose_estimates;

  // --- Getting new detections ---

  for (auto& mod : vision_modules) {
    if (mod->has_new_detections()) {
      for (const auto [timestamp, det] : mod->get_detections()) {
        const auto& [tag_id, estimate] = det;
        pose_estimates.emplace_back(mod->get_robot_to_camera_transform(), std::make_tuple(timestamp, tag_id, estimate));
      }
    }
  }

  std::map<units::second_t, frc::Pose3d> pose_evaluations;

  // --- Resolving tag pose ambiguities ---

  for (const auto& [robot_to_camera, pose_estimate] : pose_estimates) {
    const auto& [timestamp, tag_id, estimate] = pose_estimate;

    std::optional<frc::Pose3d> _pose1 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose1),
                               _pose2 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose2);

    frc::Pose3d pose1 = _pose1.value_or(frc::Pose3d()),
                pose2 = _pose2.value_or(frc::Pose3d());

    bool pose1_good = _pose1.has_value(),
         pose2_good = _pose2.has_value();

    if (estimate.GetAmbiguity() <= 0.2) {
      // No ambiguity.
      bool which_pose = estimate.error1 <= estimate.error2;
      if (which_pose && pose1_good) {
        pose_evaluations.emplace(timestamp, pose1);
      }
      else if (!which_pose && pose2_good) {
        pose_evaluations.emplace(timestamp, pose2);
      }
      continue;
    }

    // Ambiguity D:

    pose1_good = pose1_good && is_pose_on_field(pose1);
    pose2_good = pose2_good && is_pose_on_field(pose2);

    if (pose1_good && !pose2_good) {
      pose_evaluations.emplace(timestamp, pose1);
      continue;
    }
    else if (!pose1_good && pose2_good) {
      pose_evaluations.emplace(timestamp, pose2);
      continue;
    }
  }

  // --- Validating poses ---

  std::map<units::second_t, frc::Pose2d> pose_evaluations_2d;

  for (const auto& [timestamp, pose] : pose_evaluations) {
    if (is_pose_on_field(pose)) {
      pose_evaluations_2d.emplace(timestamp, frc::Pose2d(pose.X(), pose.Y(), pose.Rotation().Z()));
    }
  }

  // --- Sending poses ---

  if (!pose_evaluations_2d.empty()) {
    std::shared_ptr<nt::NetworkTable> poses_subtable = NTHandler::get()->get_rasp_table()->GetSubTable("Poses");
    for (const auto& [timestamp, pose] : pose_evaluations_2d) {
      poses_subtable->PutString(std::to_string(timestamp.value()), fmt::format("{},{},{}", pose.X().value(), pose.Y().value(), pose.Rotation().Radians().value()));
      
      NTHandler::get()->get_smart_dashboard()->PutNumber("thunderdashboard_drive_x_pos", pose.X().value());
      NTHandler::get()->get_smart_dashboard()->PutNumber("thunderdashboard_drive_y_pos", pose.Y().value());
      NTHandler::get()->get_smart_dashboard()->PutNumber("thunderdashboard_drive_ang", pose.Rotation().Radians().value());
    }
  }
}

std::optional<frc::Pose3d> Vision::calculate_robot_pose(int tag_id, frc::Transform3d robot_to_tag) {
  std::optional<frc::Pose3d> tag_pose = vision_settings.field_layout->GetTagPose(tag_id);
  if (!tag_pose.has_value()) return std::nullopt;

  const frc::Transform3d tag_to_robot = robot_to_tag.Inverse();
  return tag_pose.value() + tag_to_robot;
}

frc::Pose3d Vision::calculate_tag_pose(frc::Pose2d _robot_pose, frc::Transform3d robot_to_tag) {
  const frc::Pose3d robot_pose(_robot_pose);
  return robot_pose + robot_to_tag;
}

bool Vision::is_pose_on_field(frc::Pose3d pose) {
  const units::meter_t tol = vision_settings.robot_pose_tolerance;

  const units::meter_t min_z = 0_m - tol, max_z = vision_settings.max_robot_elevation + tol;
  const units::meter_t min_x = 0_m - tol, max_x = 8.2296_m + tol;
  const units::meter_t min_y = 0_m - tol, max_y = 16.4592_m + tol;

  const units::meter_t x = pose.X(), y = pose.Y(), z = pose.Z();
  
  return (x >= min_x && x <= max_x) &&
         (y >= min_y && y <= max_y) &&
         (z >= min_z && z <= max_z);
}
