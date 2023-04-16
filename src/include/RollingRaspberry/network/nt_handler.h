#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

class NTHandler {
public:
  static NTHandler* get() {
    return &instance;
  }
  
  NTHandler(NTHandler const&) = delete;
  NTHandler& operator=(NTHandler const&) = delete;
  
  void init();
  
  enum class MatchMode {
    DISABLED,
    AUTO,
    TELEOP,
    TEST,
    ESTOPPED,
  };
  
  MatchMode get_match_mode();
  
  enum class Alliance {
    BLUE,
    RED,
  };
  
  Alliance get_alliance();
  
  enum class MatchType {
    NONE,
    PRACTICE,
    QUAL,
    ELIM,
  };
  
  MatchType get_match_type();
  
  std::size_t get_station_number();
  
  std::string get_event_name();
  std::string get_game_msg();
  std::size_t get_match_number();
  std::size_t get_replay_number();

  bool is_connected();
  
  inline nt::NetworkTableInstance get_instance() { return nt_inst; }
  
  // The table used by the Dashboard and Robot to provide feedback to each other.
  inline std::shared_ptr<nt::NetworkTable> get_smart_dashboard() { return sd_table; }
  // The table used by FMS to provide feedback to the Robot.
  inline std::shared_ptr<nt::NetworkTable> get_fms_info() { return fms_table; }
  // The table used by this pi to provide feedback to the Robot.
  inline std::shared_ptr<nt::NetworkTable> get_rasp_table() { return rasp_table; }
  
private:
  NTHandler();
  ~NTHandler();
  
  nt::NetworkTableInstance nt_inst;
  std::shared_ptr<nt::NetworkTable> sd_table, fms_table, rasp_table;
  
  int32_t get_ctrl_word();
  
  static NTHandler instance;
};
