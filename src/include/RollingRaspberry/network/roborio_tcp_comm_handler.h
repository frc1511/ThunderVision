#pragma once

#include <RollingRaspberry/rolling_raspberry.h>
#include <frc/geometry/Pose3d.h>
#include <units/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

class RoboRIOTCPCommHandler {
public:
  static RoboRIOTCPCommHandler* get() {
    return &instance;
  }
  
  RoboRIOTCPCommHandler(RoboRIOTCPCommHandler const&) = delete;
  RoboRIOTCPCommHandler& operator=(RoboRIOTCPCommHandler const&) = delete;
  
  void init();
  void process();
  
  void send_data(char* buf, std::size_t buf_size);
  
  void send_vision_data(units::second_t time_since_data, frc::Pose3d pose1, double pose1_error, frc::Pose3d pose2, double pose2_error);
  
private:
  RoboRIOTCPCommHandler() = default;
  ~RoboRIOTCPCommHandler() = default;

  void connect_to_server();
  
  sockaddr_in server_addr;
  int client_fd = 0;
  
  static RoboRIOTCPCommHandler instance;
};
