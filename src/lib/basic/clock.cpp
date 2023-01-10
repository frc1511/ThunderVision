#include <RollingRaspberry/basic/clock.h>

Clock::Clock() {
  start_time_point = std::chrono::high_resolution_clock::now();
}

Clock::~Clock() { }

units::second_t Clock::get_uptime() const {
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time_point).count() * 1e-9);
}

units::second_t Clock::get_time_since_epoch() const {
  const auto now = std::chrono::high_resolution_clock::now();
  
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 1e-9);
}

Clock Clock::instance {};
