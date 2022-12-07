#pragma once

#include <RollingRaspberry/network/nt_handler.h>
#include <RollingRaspberry/basic/robot_settings.h>

class Subsystem {
public:
  virtual void reset_to_mode(NTHandler::MatchMode mode) = 0;
  virtual void process() = 0;
  
  virtual void send_feedback() { }
  
protected:
  static inline RobotSettings robot_settings;
};
