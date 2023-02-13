#include <RollingRaspberry/network/roborio_tcp_comm_handler.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/*
  Message structure:
  
  pose1_x     - 4 bytes (float)
  pose1_y     - 4 bytes (float)
  pose1_z     - 4 bytes (float)
  pose1_qw    - 4 bytes (float)
  pose1_qx    - 4 bytes (float)
  pose1_qy    - 4 bytes (float)
  pose1_qz    - 4 bytes (float)
  pose1_error - 4 bytes (float)
  pose2_x     - 4 bytes (float)
  pose2_y     - 4 bytes (float)
  pose2_z     - 4 bytes (float)
  pose2_qw    - 4 bytes (float)
  pose2_qx    - 4 bytes (float)
  pose2_qy    - 4 bytes (float)
  pose2_qz    - 4 bytes (float)
  pose2_error - 4 bytes (float)
*/
#define VISION_MESSAGE_SIZE (sizeof(float) * 16)

void RoboRIOTCPCommHandler::init() {
SOCKET_CREATE:
  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd <= 0) {
    fmt::print(FRC1511_LOG_PREFIX "socket() failed: {}\n", strerror(errno));
    goto SOCKET_CREATE;
  }
  
  fmt::print(FRC1511_LOG_PREFIX "Created socket\n");
  
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  
  int port = 5802;
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port); // TODO: Make configurable
  inet_pton(AF_INET, "roboRIO-1511-frc.local", &server_addr.sin_addr);
  std::memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
}

void RoboRIOTCPCommHandler::process() {
  // Try to read purely to check connection status.
  char buf[64];
  ssize_t bytes_read = read(client_fd, buf, 64);
  if (bytes_read < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return;
    }
    else if (errno == ECONNRESET) {
      fmt::print(FRC1511_LOG_PREFIX "Server connection reset\n");
    }
    else {
      fmt::print(FRC1511_LOG_PREFIX "read() failed: {}\n", strerror(errno));
    }
    // Reconnect to the server.
    connect_to_server();
  }
}

void RoboRIOTCPCommHandler::send_data(char* buf, std::size_t buf_size) {
  ssize_t bytes_written = write(client_fd, buf, buf_size);
SOCKET_WRITE:
  if (bytes_written < 0) {
    if (errno == EAGAIN) {
      goto SOCKET_WRITE;
    }
    fmt::print(FRC1511_LOG_PREFIX "write() failed: {}\n", strerror(errno));
  }
}

void RoboRIOTCPCommHandler::send_vision_data(frc::Pose3d pose1, double pose1_error, frc::Pose3d pose2, double pose2_error) {
  char buf[VISION_MESSAGE_SIZE];
  char* iter = buf;
  
  auto put_float = [&iter](float f) {
    std::memcpy(iter, &f, sizeof(float));
    iter += sizeof(float);
  };
  
  put_float(pose1.X().value());
  put_float(pose1.Y().value());
  put_float(pose1.Z().value());
  frc::Quaternion quat = pose1.Rotation().GetQuaternion();
  put_float(quat.W());
  put_float(quat.X());
  put_float(quat.Y());
  put_float(quat.Z());
  put_float(pose1_error);
  
  put_float(pose2.X().value());
  put_float(pose2.Y().value());
  put_float(pose2.Z().value());
  quat = pose2.Rotation().GetQuaternion();
  put_float(quat.W());
  put_float(quat.X());
  put_float(quat.Y());
  put_float(quat.Z());
  put_float(pose2_error);
  
  send_data(buf, VISION_MESSAGE_SIZE);
}

void RoboRIOTCPCommHandler::connect_to_server() {
RETRY_CONNECTION:
  int res = connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr));
  if (res < 0) {
    fmt::print(FRC1511_LOG_PREFIX "connect() failed: {}\n", strerror(errno));
    goto RETRY_CONNECTION;
  }
  
  fmt::print(FRC1511_LOG_PREFIX "Connected to server\n");
}

RoboRIOTCPCommHandler RoboRIOTCPCommHandler::instance;
