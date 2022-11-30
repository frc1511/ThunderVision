#pragma once

#include <RollingRaspberry/config/config.h>
#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/vision/camera.h>

class Settings {
public:
  Settings();
  ~Settings();
  
  inline std::vector<USBCameraStream>& get_usb_cameras() { return usb_cameras; }
  inline std::vector<MJPGCameraStream>& get_mjpg_cameras() { return mjpg_cameras; }
  
private:
  std::vector<USBCameraStream> usb_cameras;
  std::vector<MJPGCameraStream> mjpg_cameras;
};
