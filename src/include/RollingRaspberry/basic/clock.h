#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <units/time.h>

class Clock {
public:
  using TimePoint = std::chrono::high_resolution_clock::time_point;

  static void init();
  
  /**
   * @brief Returns the starting time point.
   *
   * @return The starting time point.
   */
  static constexpr TimePoint get_start_time_point() { return start_time_point; }
  
  /**
   * @brief Returns the uptime of the Raspberry Pi.
   *
   * @return The uptime of the Raspberry Pi.
   */
  static units::second_t get_uptime();
  
  /**
   * @brief Returns the unix timestamp in seconds.
   *
   * @return The unix timestamp in seconds.
   */
  static units::second_t get_time_since_epoch();

  /**
   * @brief Get the current time point.
   * 
   * @return The current time point.
   */
  static TimePoint get_time_point();

  /**
   * @brief Get the elapsed time since a time point.
   * 
   * @param point The time point to measure from.
   * @return The elapsed time since the time point.
   */
  static units::second_t get_time_since_point(TimePoint point);
  
private:
  static inline TimePoint start_time_point;
};
