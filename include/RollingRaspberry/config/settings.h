#pragma once

#include <RollingRaspberry/config/config.h>
#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera.h>

class Settings {
public:
  Settings();
  ~Settings();
  
private:
  std::vector<USBCameraStream> usb_cameras;
  std::vector<MJPGCameraStream> mjpg_cameras;
};
