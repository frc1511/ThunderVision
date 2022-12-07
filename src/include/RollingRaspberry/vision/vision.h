#pragma once

#include <RollingRaspberry/basic/subsystem.h>
#include <RollingRaspberry/vision/camera.h>
#include <RollingRaspberry/vision/vision_module.h>
#include <RollingRaspberry/vision/vision_settings.h>

class Vision : public Subsystem {
public:
  Vision();
  ~Vision();
  
  void reset_to_mode(NTHandler::MatchMode mode) override;
  void process() override;
  
private:
  VisionSettings vision_settings;

  std::vector<USBCameraStream> usb_cameras;
  std::vector<MJPGCameraStream> mjpg_cameras;
  
  std::vector<std::unique_ptr<VisionModule>> vision_modules;
};
