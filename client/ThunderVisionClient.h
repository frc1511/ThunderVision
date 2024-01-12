#pragma once

#include <frc/geometry/Pose3d.h>
#include <units/time.h>

#include <vector>
#include <thread>
#include <mutex>

class ThunderVisionClient final {
  const char* m_server_address;
  const int m_server_port;
  const units::second_t m_cutoff_time;

  std::thread m_thread;
  std::mutex m_terminated_mutex;

  bool m_terminated = false;

  std::mutex m_poses_mutex;
  
  struct TimestampedPoses {
    int64_t time;
    std::vector<frc::Pose3d> poses;
  };

  std::vector<TimestampedPoses> m_poses;

public:
  ThunderVisionClient(const char* server_address,
                      int server_port,
                      units::second_t cutoff_time = 50_ms);
  ~ThunderVisionClient();

  std::vector<frc::Pose3d> get_latest_poses();

private:
  void thread_main();
};
