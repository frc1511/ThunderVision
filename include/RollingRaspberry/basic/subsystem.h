#pragma once

#include <RollingRaspberry/basic/nt_handler.h>

class Subsystem {
public:
  virtual void reset_to_mode(NTHandler::MatchMode mode) = 0;
  virtual void process() = 0;
  
  virtual void send_feedback() { }
};
