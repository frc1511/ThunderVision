#pragma once

#include <frc/geometry/Pose3d.h>

#include <netinet/in.h>
#include <vector>

struct NetworkingConfig;

class Networking {
  const NetworkingConfig& m_config;

  int m_server_fd;
  sockaddr_in m_server_addr;

  bool m_connected = false;

  int m_client_fd;
  sockaddr_in m_client_addr;

public:
  Networking(const NetworkingConfig& config);
  ~Networking();

  void process();

  void send_poses(const std::vector<frc::Pose3d>& poses);
};

