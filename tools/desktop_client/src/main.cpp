#include <cstdio>

#include <ThunderVisionClient.h>

#include <chrono>
#include <thread>

#include <csignal>
#include <unistd.h>

volatile std::sig_atomic_t g_signal_status;

int main(int argc, const char** argv) {
  const auto signal_handler = [](int32_t signal) {
    write(STDERR_FILENO, "Signal received\n", 16);
    g_signal_status = signal;
  };

  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);


  const char* server_address = "127.0.0.1";
  const int server_port = 5802;

  ThunderVisionClient client(server_address, server_port);

  std::vector<frc::Pose3d> poses;
  while (g_signal_status == 0) {
    using namespace std::chrono_literals;

    const auto frame_start = std::chrono::high_resolution_clock::now();

    poses = client.get_latest_poses();

    if (!poses.empty()) {
      for (const auto& pose : poses) {
        printf("Pose: %f %f %f\n", pose.X().value(), pose.Y().value(), pose.Z().value());
      }
    }

    const auto frame_end = std::chrono::high_resolution_clock::now();

    const auto frame_duration = frame_end - frame_start;
    if (frame_duration < 20ms) {
      std::this_thread::sleep_for(20ms - frame_duration);
    }
  }

  if (g_signal_status != 0) {
    fprintf(stderr, "Signal %d received, exiting...\n", g_signal_status);
  }


  return 0;
}
