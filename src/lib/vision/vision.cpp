#include <RollingRaspberry/vision/vision.h>
#include <RollingRaspberry/network/nt_handler.h>
#include <RollingRaspberry/basic/clock.h>
#include <RollingRaspberry/network/roborio_tcp_comm_handler.h>

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
  for (auto& mod : vision_modules) {
    if (mod->has_new_detections()) {
      for (const auto [time_point, det] : mod->get_detections()) {
        const auto& [tag_id, estimate] = det;
        frc::Transform3d robot_to_camera = mod->get_robot_to_camera_transform();

        // Calculate the robot's pose for each of the two possible AprilTag transforms.
        std::optional<frc::Pose3d> _pose1 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose1),
                                   _pose2 = calculate_robot_pose(tag_id, robot_to_camera + estimate.pose2);

        frc::Pose3d pose1 = _pose1.value_or(frc::Pose3d(-1_m, -1_m, -1_m, frc::Rotation3d())),
                    pose2 = _pose2.value_or(frc::Pose3d(-1_m, -1_m, -1_m, frc::Rotation3d()));

        // If both are bad, don't send anything.
        if (!_pose1 && !_pose2) continue;

        double error1 = estimate.error1,
              error2 = estimate.error2;

        // If one is bad, adjust the errors so that the good one is always chosen.
        if (_pose1 && !_pose2) {
          error1 = 0;
          error2 = 1;
        }
        else if (!_pose1 && _pose2) {
          error1 = 1;
          error2 = 0;
        }

        RoboRIOTCPCommHandler::get()->send_vision_data(Clock::get_time_since_point(time_point), pose1, error1, pose2, error2);
      }
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
