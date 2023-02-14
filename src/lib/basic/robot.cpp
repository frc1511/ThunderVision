#include <RollingRaspberry/basic/robot.h>
#include <RollingRaspberry/network/nt_handler.h>
#include <RollingRaspberry/network/roborio_tcp_comm_handler.h>
#include <RollingRaspberry/basic/clock.h>

Robot::Robot() {
  Clock::init();
  RoboRIOTCPCommHandler::get()->init();
}

Robot::~Robot() { }

void Robot::robot_process() {
  for (Subsystem* s : subsystems) {
    s->send_feedback();
  }

  RoboRIOTCPCommHandler::get()->process();
  vision.process();
}

void Robot::auto_init() {
  reset(NTHandler::MatchMode::AUTO);
}

void Robot::auto_process() { }

void Robot::teleop_init() {
  reset(NTHandler::MatchMode::TELEOP);
}

void Robot::teleop_process() { }

void Robot::disabled_init() {
  reset(NTHandler::MatchMode::DISABLED);
}

void Robot::disabled_process() { }

void Robot::test_init() {
  reset(NTHandler::MatchMode::TEST);
}

void Robot::test_process() { }

void Robot::reset(NTHandler::MatchMode mode) {
  for (Subsystem* s : subsystems) {
    s->reset_to_mode(mode);
  }
}
