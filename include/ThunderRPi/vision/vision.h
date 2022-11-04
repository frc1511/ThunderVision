#pragma once

#include <ThunderRPi/vision/camera.h>

class Vision {
public:
  Vision();
  ~Vision();
  
  void process();
  
private:
  USBCameraStream intake_stream { 0, { "intake_camera" } };
  MJPGCameraStream limelight_stream { "http://limelight-homer.local:5800/stream.mjpg", { "limelight" } };
};
