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

  /**
   * @brief Returns the starting time point.
   *
   * @return The starting time point.
   */
  constexpr std::chrono::high_resolution_clock::time_point get_start_time_point() const { return start_time_point; }

  /**
   * @brief Returns the uptime of the robot.
   *
   * @return The uptime of the robot.
   */
  units::second_t get_uptime() const;

  /**
   * @brief Returns the unix timestamp in seconds.
   *
   * @return The unix timestamp in seconds.
   */
  units::second_t get_time_since_epoch() const;

private:
  Clock();
  ~Clock();

  std::chrono::high_resolution_clock::time_point start_time_point;

  static Clock instance;
};
