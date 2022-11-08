#pragma once

class Robot {
public:
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
