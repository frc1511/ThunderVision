#include <RollingRaspberry/basic/robot.h>
#include <RollingRaspberry/basic/nt_handler.h>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <unistd.h>

void Robot::auto_init() {

}

void Robot::auto_process() {

}

void Robot::teleop_init() {

}

void Robot::teleop_process() {

}

void Robot::disabled_init() {

}

void Robot::disabled_process() {

}

void Robot::test_init() {

}

void Robot::test_process() {

}


int main(int argc, char** argv) {
  NTHandler::get()->init();

  Robot robot;

  NTHandler::MatchMode last_mode = NTHandler::MatchMode::DISABLED;

  while (true) {
    std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now(),
                                          end;

    NTHandler::MatchMode curr_mode = NTHandler::get()->get_match_mode();
    bool init = last_mode != curr_mode;

    switch (curr_mode) {
      case NTHandler::MatchMode::DISABLED:
      case NTHandler::MatchMode::ESTOPPED:
        if (init) {
          robot.disabled_init();
        }
        robot.disabled_process();
        break;
      case NTHandler::MatchMode::AUTO:
        if (init) {
          robot.auto_init();
        }
        robot.auto_process();
        break;
      case NTHandler::MatchMode::TELEOP:
        if (init) {
          robot.teleop_init();
        }
        robot.teleop_process();
        break;
      case NTHandler::MatchMode::TEST:
        if (init) {
          robot.test_init();
        }
        robot.test_process();
        break;
    }

    last_mode = curr_mode;

    std::size_t dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (dur < 20) {
      std::size_t sleep_time = 20 - dur;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
  }

  return 0;
}
