#include <RollingRaspberry/basic/clock.h>

Clock::Clock() {
  start_time_point = std::chrono::steady_clock::now();
}

Clock::~Clock() { }

units::second_t Clock::get_uptime() const {
  return units::second_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time_point).count() * 0.001);
}

Clock Clock::instance {};
