#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <units/time.h>

class Clock {
public:
  static Clock* get() {
    return &instance;
  }

  Clock(Clock const&) = delete;
  void operator=(Clock const&) = delete;

  constexpr std::chrono::steady_clock::time_point get_start_time_point() const { return start_time_point; }

  units::second_t get_uptime() const;

private:
  Clock();
  ~Clock();

  std::chrono::steady_clock::time_point start_time_point;

  static Clock instance;
};