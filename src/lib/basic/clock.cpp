#include <RollingRaspberry/basic/clock.h>
#include <RollingRaspberry/network/nt_handler.h>

Clock::Clock() { }

Clock::~Clock() { }

void Clock::init() {
  start_time_point = std::chrono::high_resolution_clock::now();
  
  roborio_timestamp_subscriber = NTHandler::get()->get_rasp_table()->GetDoubleTopic("roboRIO_FPGA_Timestamp").Subscribe({});
  roborio_timestamp_listener = NTHandler::get()->get_instance().AddListener(
    roborio_timestamp_subscriber,
    nt::EventFlags::kValueAll,
    [this](const nt::Event& event) {
      update_roborio_timestamp(event);
    }
  );
}

units::second_t Clock::get_uptime() const {
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time_point).count() * 1e-9);
}

units::second_t Clock::get_roborio_uptime_offset() const {
  std::lock_guard<std::mutex> lk(mut);
  return roborio_time_offset;
}

units::second_t Clock::get_roborio_uptime() const {
  std::lock_guard<std::mutex> lk(mut);
  return units::microsecond_t(nt::Now()) + roborio_time_offset;
}

units::second_t Clock::get_time_since_epoch() const {
  const auto now = std::chrono::high_resolution_clock::now();
  
  return units::second_t(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() * 1e-9);
}

void Clock::update_roborio_timestamp(const nt::Event& event) {
  /* units::second_t time_since_value = units::microsecond_t(nt::Now() - event.GetValueEventData()->value.time()); */
  
  // Add to local time to get equivalent server time.
  units::microsecond_t offset(event.GetTimeSyncEventData()->serverTimeOffset);
  
  std::lock_guard<std::mutex> lk(mut);
  roborio_time_offset = offset;
}


Clock Clock::instance {};
