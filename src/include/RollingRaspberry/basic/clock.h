#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <networktables/NetworkTableListener.h>
#include <networktables/DoubleTopic.h>
#include <units/time.h>

class Clock {
public:
  static Clock* get() {
    return &instance;
  }
  
  Clock(Clock const&) = delete;
  void operator=(Clock const&) = delete;
  
  void init();
  
  /**
   * @brief Returns the starting time point.
   *
   * @return The starting time point.
   */
  constexpr std::chrono::high_resolution_clock::time_point get_start_time_point() const { return start_time_point; }
  
  /**
   * @brief Returns the uptime of the Raspberry Pi.
   *
   * @return The uptime of the Raspberry Pi.
   */
  units::second_t get_uptime() const;
  
  /**
   * @brief Returns the offset of the roboRIO time.
   *
   * @return The offset of the roboRIO time.
   */
  units::second_t get_roborio_uptime_offset() const;

  /**
   * @brief Returns the roboRIO time.
   *
   * @return The roboRIO time.
   */
  units::second_t get_roborio_uptime() const;
  
  /**
   * @brief Returns the unix timestamp in seconds.
   *
   * @return The unix timestamp in seconds.
   */
  units::second_t get_time_since_epoch() const;
  
private:
  Clock();
  ~Clock();
  
  /**
   * @brief Callback function for roboRIO timestamp listener.
   *
   * @param event The Network Tables event.
   */
  void update_roborio_timestamp(const nt::Event& event);
  
  std::chrono::high_resolution_clock::time_point start_time_point;
  
  mutable std::mutex mut;
  units::second_t roborio_time_offset = 0_s;
  
  nt::DoubleSubscriber roborio_timestamp_subscriber;
  NT_Listener roborio_timestamp_listener;
  
  static Clock instance;
};
