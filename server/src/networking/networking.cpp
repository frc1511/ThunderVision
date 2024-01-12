#include <thunder_vision/networking/networking.hpp>

#include <thunder_vision/diagnostics.hpp>
#include <thunder_vision/networking/networking_config.hpp>

#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

Networking::Networking(const NetworkingConfig& config)
  : m_config(config) {

  using namespace std::chrono_literals;

socket_create:
  m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_server_fd < 0) {
    fprintf(stderr, ERROR_TAG "Failed to create socket: %s\n", strerror(errno));
    std::this_thread::sleep_for(500ms);
    goto socket_create;
  }

  puts(NETWORKING_TAG "Created socket");

  // Enable non-blocking io.
  int flags = fcntl(m_server_fd, F_GETFL, 0);
  fcntl(m_server_fd, F_SETFL, flags | O_NONBLOCK);

  m_server_addr.sin_family = AF_INET;
  m_server_addr.sin_addr.s_addr = INADDR_ANY;
  m_server_addr.sin_port = htons((int)m_config.port);
  std::memset(m_server_addr.sin_zero, 0, sizeof(m_server_addr.sin_zero));

socket_bind:
  if (bind(m_server_fd, (struct sockaddr*)&m_server_addr,
           sizeof(m_server_addr)) < 0) {
    fprintf(stderr, ERROR_TAG "Failed to bind socket: %s\n", strerror(errno));
    std::this_thread::sleep_for(1s);
    goto socket_bind;
  }

  puts(NETWORKING_TAG "Bound socket");

socket_listen:
  if (listen(m_server_fd, 1) < 0) {
    fprintf(stderr, ERROR_TAG "Failed to listen on socket: %s\n",
            strerror(errno));
    std::this_thread::sleep_for(1s);
    goto socket_listen;
  }

  puts(NETWORKING_TAG "Listening on socket");
}

Networking::~Networking() {
  if (m_connected) {
    puts(NETWORKING_TAG "Closing connection to client");
    close(m_client_fd);
  }

  close(m_server_fd);
}

void Networking::process() {
  // Only one client should be connecting the server, so if we can accept a new
  // connection we should close the old one.
  {
    sockaddr_in new_client_addr;
    socklen_t new_client_addr_len = sizeof(new_client_addr);
    int new_client_fd = accept(m_server_fd, (struct sockaddr*)&new_client_addr,
                               &new_client_addr_len);
    if (new_client_fd < 0) {
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        fprintf(stderr, ERROR_TAG "Failed to accept client: %s\n",
                strerror(errno));
      }
    } else {
      puts(NETWORKING_TAG "Accepted client");

      // Get rid of the previous connection.
      if (m_connected) {
        puts(NETWORKING_TAG "Closing previous connection");
        close(m_client_fd);
      }

      m_connected = true;

      m_client_fd = new_client_fd;
      m_client_addr = new_client_addr;
    }
  }
}

void Networking::send_poses(const std::vector<frc::Pose3d>& poses) {
  if (!m_connected) return;

  // Send 1 byte regardless of whether or not we have any poses to send.
  // This is to check whether or not the client is still connected.

  char buffer[2048];
  std::memcpy(buffer + 1, poses.data(), sizeof(frc::Pose3d) * poses.size());
  const size_t buffer_size = 1 + sizeof(frc::Pose3d) * poses.size();

  ssize_t bytes_sent = write(m_client_fd, buffer, buffer_size);
  if (bytes_sent < 0) {
    fprintf(stderr, ERROR_TAG "Failed to write to client: %s\n",
            strerror(errno));
    fputs(ERROR_TAG "Closing connection to client\n", stderr);
    m_connected = false;
    close(m_client_fd);
    return;
  }
}

