#include <RollingRaspberry/network/nt_handler.h>

extern "C" {
  struct HAL_ControlWord {
    uint32_t enabled : 1;
    uint32_t autonomous : 1;
    uint32_t test : 1;
    uint32_t eStop : 1;
    uint32_t fmsAttached : 1;
    uint32_t dsAttached : 1;
    uint32_t control_reserved : 26;
  };
  typedef struct HAL_ControlWord HAL_ControlWord;
}

NTHandler::NTHandler() = default;

NTHandler::~NTHandler() = default;

void NTHandler::init() {
  nt_inst = nt::NetworkTableInstance::GetDefault();
  sd_table = nt_inst.GetTable("SmartDashboard");
  fms_table = nt_inst.GetTable("FMSInfo");
  rasp_table = nt_inst.GetTable("RollingRaspberry");
  
  nt_inst.StartClient4("RollingRaspberry");
  nt_inst.SetServerTeam(1511);
}

NTHandler::MatchMode NTHandler::get_match_mode() {
  int32_t ctrl_word_int = get_ctrl_word();
  
  HAL_ControlWord ctrl_word;
  std::memcpy(&ctrl_word, &ctrl_word_int, sizeof(ctrl_word));

  if (!ctrl_word.enabled) {
    return MatchMode::DISABLED;
  }
  else if (ctrl_word.autonomous) {
    return MatchMode::AUTO;
  }
  else if (ctrl_word.test) {
    return MatchMode::TEST;
  }
  else if (ctrl_word.eStop) {
    return MatchMode::ESTOPPED;
  }
  else {
    return MatchMode::TELEOP;
  }
}

NTHandler::Alliance NTHandler::get_alliance() {
  if (static_cast<bool>(fms_table->GetNumber("IsRedAlliance", true))) {
    return Alliance::RED;
  }
  else {
    return Alliance::BLUE;
  }
}

NTHandler::MatchType NTHandler::get_match_type() {
  return static_cast<NTHandler::MatchType>(fms_table->GetNumber("MatchType", 0));
}

std::size_t NTHandler::get_station_number() {
  return static_cast<std::size_t>(fms_table->GetNumber("StationNumber", 1.0));
}

std::string NTHandler::get_event_name() {
  return fms_table->GetString("EventName", "");
}

std::string NTHandler::get_game_msg() {
  return fms_table->GetString("GameSpecificMessage", "");
}

std::size_t NTHandler::get_match_number() {
  return fms_table->GetNumber("MatchNumber", 0.0);
}

std::size_t NTHandler::get_replay_number() {
  return fms_table->GetNumber("ReplayNumber", 0.0);
}

int32_t NTHandler::get_ctrl_word() {
  return static_cast<int32_t>(fms_table->GetNumber("FMSControlData", 0.0));
}

NTHandler NTHandler::instance;
