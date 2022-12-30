#pragma once

#include <RollingRaspberry/basic/subsystem.h>
#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/vision/vision_module.h>
#include <RollingRaspberry/vision/vision_settings.h>
#include <frc/geometry/Transform3d.h>
#include <frc/geometry/Transform2d.h>
#include <frc/geometry/Rotation3d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Pose3d.h>
#include <frc/geometry/Pose2d.h>
#include <units/length.h>
#include <units/angle.h>

class Vision : public Subsystem {
public:
  Vision();
  ~Vision();
  
  void reset_to_mode(NTHandler::MatchMode mode) override;
  void process() override;
  
private:
  /**
   * @brief Calculates the robot pose from the apriltag pose.
   * 
   * @param robot_to_camera Transform from the robot center to the camera.
   * @param camera_to_tag Transform from the camera to the AprilTag.
   * @param tag_pose Known pose of the AprilTag.
   * @return The calculated robot pose.
   */
  static frc::Pose3d calculate_robot_pose(frc::Transform3d robot_to_camera, frc::Transform3d camera_to_tag, frc::Pose3d tag_pose);

  /**
   * @brief Calculates the expected pose of the AprilTag.
   * 
   * @param robot_pose Robot pose.
   * @param robot_to_camera Transform from the robot center to the camera.
   * @param camera_to_tag Transform from the camera to the AprilTag.
   * @return The calculated AprilTag pose.
   */
  static frc::Pose3d calculate_tag_pose(frc::Pose2d robot_pose, frc::Transform3d robot_to_camera, frc::Transform3d camera_to_tag);

  /**
   * @brief Returns the last robot pose (retreived from network tables).
   * 
   * @return The last robot pose.
   */
  static frc::Pose2d get_last_robot_pose();

  VisionSettings vision_settings;

  std::vector<USBCameraStream> usb_cameras;
  std::vector<MJPGCameraStream> mjpg_cameras;
  
  std::vector<std::unique_ptr<VisionModule>> vision_modules;
};
