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
  // --- Resolving tag pose estimates that are within the delay from vision modules ---

  using PoseEstimation = std::pair<int, frc::AprilTagPoseEstimate>;
  std::vector<std::pair<frc::Transform3d, PoseEstimation>> pose_estimates;

  std::vector<frc::Pose3d> pose_evaluations;

  using namespace std::chrono_literals;
  std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds max_frame_delay = std::chrono::milliseconds(static_cast<std::size_t>(vision_settings.max_frame_delay * 1000));

  for (auto& mod : vision_modules) {
    if (mod->has_new_detections()) {
      // If time since last pose estimate is less than or equal to delay, add to vector.
      if (now - mod->get_detections_time_point() <= max_frame_delay) {
        for (auto& det : mod->get_detections()) {
          pose_estimates.emplace_back(mod->get_robot_to_camera_transform(), det);
        }
      }
    }
  }

  // --- Resolving tag pose transform ambiguities ---

  frc::Pose2d last_robot_pose = get_last_robot_pose();

  for (const auto& [robot_to_camera, pose_estimate] : pose_estimates) {
    const auto& [tag_id, estimate] = pose_estimate;

    const frc::Pose3d pose1 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose1),
                      pose2 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose2);

    if (estimate.GetAmbiguity() <= 0.2) {
      // No ambiguity.
      pose_evaluations.push_back((estimate.error1 <= estimate.error2) ? pose1 : pose2);
      continue;
    }

    // Ambiguity D:

    const bool pose1_grounded = is_within_vertical_tolerance(pose1),
               pose2_grounded = is_within_vertical_tolerance(pose2);

    if (pose1_grounded && !pose2_grounded) {
      pose_evaluations.push_back(pose1);
      continue;
    }
    else if (!pose1_grounded && pose2_grounded) {
      pose_evaluations.push_back(pose2);
      continue;
    }

    // If they are really close together
    if (pose1.Translation().Distance(pose2.Translation()) < vision_settings.robot_pose_tolerance) {
      // Modpoint?
      pose_evaluations.push_back(pose1 + (pose2 - pose1) / 2);
      continue;
    }
  }

  std::vector<frc::Pose2d> pose_evaluations_2d;

  for (auto& pose : pose_evaluations) {
    if (is_within_vertical_tolerance(pose)) {
      pose_evaluations_2d.emplace_back(pose.X(), pose.Y(), pose.Rotation().Z());
    }
  }

  {
    // TODO: Find outliers and remove them.
    std::vector<frc::Translation2d> translations;
    for (auto& pose : pose_evaluations_2d) {
      translations.push_back(pose.Translation() - last_robot_pose.Translation());
    }
  }

  if (!pose_evaluations_2d.empty()) {
    std::vector<std::string> pose_strs;
    for (auto& pose : pose_evaluations_2d) {
      pose_strs.push_back(fmt::format("{},{},{}", pose.X().value(), pose.Y().value(), pose.Rotation().Radians().value()));
    }

    NTHandler::get()->get_rasp_table()->PutNumber("PoseTime", Clock::get()->get_uptime().value());
    NTHandler::get()->get_rasp_table()->PutStringArray("Poses", pose_strs);
  }
}

frc::Pose3d Vision::calculate_robot_pose(int tag_id, frc::Transform3d robot_to_tag) {
  frc::Pose3d tag_pose = vision_settings.field_layout->GetTagPose(tag_id).value();
  const frc::Transform3d tag_to_robot = robot_to_tag.Inverse();
  return tag_pose + tag_to_robot;
}

frc::Pose3d Vision::calculate_tag_pose(frc::Pose2d _robot_pose, frc::Transform3d robot_to_tag) {
  const frc::Pose3d robot_pose(_robot_pose);
  return robot_pose + robot_to_tag;
}

frc::Pose2d Vision::get_last_robot_pose() {
  auto sd = NTHandler::get()->get_smart_dashboard();

  units::meter_t x = units::meter_t(sd->GetNumber("robot_pose_x", 0.0));
  units::meter_t y = units::meter_t(sd->GetNumber("robot_pose_y", 0.0));
  frc::Rotation2d rot = frc::Rotation2d(units::degree_t(sd->GetNumber("robot_pose_rot", 0.0)));

  return frc::Pose2d(x, y, rot);
}

bool Vision::is_within_vertical_tolerance(frc::Pose3d robot_pose) {
  if (robot_pose.Z() - vision_settings.max_robot_elevation > vision_settings.robot_pose_tolerance) return false;
  if (robot_pose.Z() < -vision_settings.robot_pose_tolerance) return false;
  return true;
}
