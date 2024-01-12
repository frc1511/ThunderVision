#include <thunder_vision/config.hpp>
#include <thunder_vision/networking/networking.hpp>
#include <thunder_vision/vision/vision.hpp>

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

  const Config config = Config::load(argc, argv);

  Networking networking {config.networking_config};
  Vision vision {config.vision_config, config.field_layout, networking};

  while (g_signal_status == 0) {
    using namespace std::chrono_literals;

    const auto frame_start = std::chrono::high_resolution_clock::now();

    networking.process();
    vision.process();

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

