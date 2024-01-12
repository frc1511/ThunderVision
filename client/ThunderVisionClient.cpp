#include "ThunderVisionClient.h"

#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Milliseconds since epoch...
static int64_t get_timestamp() {
  const auto time_since_epoch =
      std::chrono::system_clock::now().time_since_epoch();
  const auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch);
  return milliseconds.count();
}

ThunderVisionClient::ThunderVisionClient(const char* server_address,
                                         int server_port,
                                         units::second_t cutoff_time)
  : m_server_address(server_address),
    m_server_port(server_port),
    m_cutoff_time(cutoff_time),
    m_thread([this]() { this->thread_main(); }) {}

ThunderVisionClient::~ThunderVisionClient() {
  {
    std::lock_guard<std::mutex> lk(m_terminated_mutex);
    m_terminated = true;
  }

  m_thread.join();
}

std::vector<frc::Pose3d> ThunderVisionClient::get_latest_poses() {
  std::lock_guard<std::mutex> lk(m_poses_mutex);

  if (m_poses.empty()) return {};

  // Remove all old poses.
  {
    const int64_t current_timestamp = get_timestamp();
    const int64_t cutoff_timestamp =
        current_timestamp - units::millisecond_t(m_cutoff_time).value();

    size_t i = 0;
    while (i < m_poses.size() && cutoff_timestamp > m_poses[i].time) {
      ++i;
    }

    m_poses.erase(m_poses.begin(), std::next(m_poses.begin(), i));
  }

  // Acquire good poses.
  std::vector<frc::Pose3d> poses;
  for (const TimestampedPoses& pose : m_poses) {
    poses.reserve(poses.size() + pose.poses.size());
    poses.insert(poses.end(), pose.poses.begin(), pose.poses.end());
  }

  return poses;
}

void ThunderVisionClient::thread_main() {
  int client_fd = -1;
  sockaddr_in server_addr;

  bool connected = false;

  unsigned char buffer[2048];
  std::memset(buffer, 0, sizeof(buffer));

  while (true) {
    {
      std::lock_guard<std::mutex> lk(m_terminated_mutex);
      if (m_terminated) break;
    }

    // Initialize client socket.
    if (client_fd < 0) {
      connected = false;

      client_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (client_fd < 0) continue;

      // Enable non-blocking io.
      int flags = fcntl(client_fd, F_GETFL, 0);
      fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

      // Server address.
      server_addr.sin_family = AF_INET,
      server_addr.sin_port = htons(m_server_port);
      inet_pton(AF_INET, m_server_address, &server_addr.sin_addr);
      std::memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
    }

    // Connect to the server!
    if (!connected) {
    connect:
      int res = connect(client_fd, (struct sockaddr*)&server_addr,
                        sizeof(server_addr));
      if (res < 0) {
        if (errno == EINPROGRESS) goto connect;
        continue;
      } else
        printf("[ThunderVision] Connected to server!\n");
    }

    const int64_t timestamp = get_timestamp();

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) continue;

      printf("[ThunderVision] Failed to read from server: %s\n",
             strerror(errno));
      printf("[ThunderVision] Resetting connection to server...\n");

      close(client_fd);
      client_fd = -1;
      connected = false;
      continue;
    }

    // Read poses!
    if (bytes_read % sizeof(frc::Pose3d) != 0) {
      printf("[ThunderVision] Invalid buffer size read from server!\n");
      continue;
    }

    const size_t num_poses = bytes_read / sizeof(frc::Pose3d);
    const std::vector<frc::Pose3d> poses(
        reinterpret_cast<frc::Pose3d*>(buffer),
        reinterpret_cast<frc::Pose3d*>(buffer) + num_poses);

    {
      std::lock_guard<std::mutex> lk(m_poses_mutex);
      m_poses.push_back({timestamp, std::move(poses)});
    }

    std::memset(buffer, 0, sizeof(buffer));
  }

  if (client_fd >= 0) {
    close(client_fd);
  }
}
 
