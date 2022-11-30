#pragma once

#include <RollingRaspberry/config/settings.h>
#include <RollingRaspberry/basic/subsystem.h>

#include <apriltag.h>
#include <tag16h5.h>

#include <opencv2/opencv.hpp>
#include <cscore.h>
#include <cscore_cv.h>

class Vision : public Subsystem {
public:
  Vision(Settings& settings);
  ~Vision();
  
  void reset_to_mode(NTHandler::MatchMode mode) override;
  void process() override;
  
private:
  std::vector<USBCameraStream>& usb_cameras;
  std::vector<MJPGCameraStream>& mjpg_cameras;
  
  apriltag_family_t* tag_family;
  apriltag_detector_t* tag_detector;
};
