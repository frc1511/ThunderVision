#include <RollingRaspberry/basic/robot.h>
#include <RollingRaspberry/basic/nt_handler.h>

#include <unistd.h>

namespace ThunderSignal {
  volatile std::sig_atomic_t status;
}

int main(int argc, char** argv) {
  auto sig_handler = [](int32_t sig) {
    ThunderSignal::status = sig;
  };
  
  std::signal(SIGINT, sig_handler); 
  std::signal(SIGTERM, sig_handler);
  
  NTHandler::get()->init();
  
  Robot robot;

  robot.robot_init();
  
  NTHandler::MatchMode last_mode = NTHandler::MatchMode::DISABLED;
  
  while (!ThunderSignal::status) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now(),
                                                   end;

    // Called every iteration, no matter what.
    robot.robot_process();
    
    NTHandler::MatchMode curr_mode = NTHandler::get()->get_match_mode();
    bool init = last_mode != curr_mode;
    
    switch (curr_mode) {
      case NTHandler::MatchMode::AUTO:
        if (init)
          robot.auto_init();
        robot.auto_process();
        break;
      case NTHandler::MatchMode::TELEOP:
        if (init)
          robot.teleop_init();
        robot.teleop_process();
        break;
      case NTHandler::MatchMode::TEST:
        if (init)
          robot.test_init();
        robot.test_process();
        break;
      default:
        if (init)
          robot.disabled_init();
        robot.disabled_process();
        break;
    }
    
    last_mode = curr_mode;
    
    // Loop time - The loop won't re-run until 20ms has elapsed.
    
    std::size_t dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    if (dur < 20) {
      std::size_t sleep_time = 20 - dur;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
  }
  
  if (ThunderSignal::status) {
    fmt::print("\nFRC1511: Caught signal {}, exiting now.\n", ThunderSignal::status);
  }
  
  return 0;
}
