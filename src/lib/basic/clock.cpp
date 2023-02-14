#include <RollingRaspberry/basic/clock.h>
#include <RollingRaspberry/network/nt_handler.h>

void Clock::init() {
  start_time_point = std::chrono::high_resolution_clock::now();
}

units::second_t Clock::get_uptime() {
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time_point).count() * 1e-9);
}

units::second_t Clock::get_time_since_epoch() {
  const auto now = std::chrono::high_resolution_clock::now();
  
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 1e-9);
}

Clock::TimePoint Clock::get_time_point() {
  return std::chrono::high_resolution_clock::now();
}

units::second_t Clock::get_time_since_point(TimePoint point) {
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - point).count() * 1e-9);
}
