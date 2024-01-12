#pragma once

#include <cstdint>
#include <filesystem>

struct NetworkingConfig {
  uint16_t port;

  [[nodiscard]] static NetworkingConfig
  load(const std::filesystem::path& config_path);
};

