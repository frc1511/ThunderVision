#include <thunder_vision/networking/networking_config.hpp>

#include <thunder_vision/diagnostics.hpp>

#include <wpi/json.h>
#include <wpi/raw_istream.h>

#include <cstdio>

#define CONFIG_FILE_NAME "networking.json"

static void from_json(const wpi::json& j, NetworkingConfig& config) {
  config.port = j.at("port").get<uint16_t>();
}

NetworkingConfig
NetworkingConfig::load(const std::filesystem::path& config_path) {
  const std::filesystem::path vision_config_file =
      config_path / CONFIG_FILE_NAME;

  std::error_code error;
  wpi::raw_fd_istream stream(vision_config_file.c_str(), error);

  if (error) {
    fprintf(stderr, ERROR_TAG "Failed to open " CONFIG_FILE_NAME " file: %s\n",
            error.message().c_str());
    exit(1);
  }

  NetworkingConfig config;
  try {
    wpi::json json;
    stream >> json;
    config = json.get<NetworkingConfig>();

  } catch (const wpi::json::exception& e) {
    fprintf(stderr, ERROR_TAG "Failed to parse " CONFIG_FILE_NAME " file: %s\n",
            e.what());
    exit(1);
  }

  return config;
}

