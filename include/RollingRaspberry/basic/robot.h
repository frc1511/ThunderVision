#pragma once

#include <RollingRaspberry/rolling_raspberry.h>

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

};
