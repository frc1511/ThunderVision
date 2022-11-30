#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <RollingRaspberry/config/settings.h>
#include <RollingRaspberry/basic/subsystem.h>
#include <RollingRaspberry/vision/vision.h>

class Robot {
public:
  void robot_init();
  void robot_process();

  void auto_init();
  void auto_process();
  
  void teleop_init();
  void teleop_process();
  
  void disabled_init();
  void disabled_process();
  
  void test_init();
  void test_process();
  
private:
  Settings settings;
  Vision vision { settings };

  std::vector<Subsystem*> subsystems {
    &vision
  };

  void reset(NTHandler::MatchMode mode);
};
