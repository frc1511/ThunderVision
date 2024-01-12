#include <thunder_vision/config.hpp>

#include <thunder_vision/diagnostics.hpp>

#include <wpi/json.h>
#include <wpi/raw_istream.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string_view>

//
// Options.
//
#define OPTION_HELP       "--help"
#define OPTION_CONFIG_DIR "--config_dir"

//
// Environment variables.
//
#define ENV_CONFIG_DIR "THUNDER_VISION_CONFIG_DIR"

// Prints the usage message and exits the program.
[[noreturn]] static void usage() {
  // clang-format off
  fputs("Usage: thunder_vision [options]\n"
        "Options:\n"
        "  --help                 Show this help message\n"
        "  --config_dir=<path>    Set the directory to load configuration files from\n",
        stderr);
  // clang-format on

  exit(1);
}

// Compares the current argument with the given option name. If the argument
// matches the option name, then a value is attempted to be parsed and returned.
// If the argument does not match the option name, then nullptr is returned.
[[nodiscard]] static const char*
get_option_value(const std::string_view option_name, uint32_t& i,
                 const uint32_t argc, const char** argv) {
  const std::string_view argument(argv[i]);

  const std::size_t argument_len = argument.length();
  const std::size_t name_len = option_name.length();

  if (argument_len < name_len) return nullptr;
  if (!argument.starts_with(option_name)) return nullptr;

  const auto missing_argument = [option_name]() {
    fprintf(stderr, ERROR_TAG "Missing argument for option '%s'\n",
            option_name.data());
    exit(1);
  };

  if (argument_len == name_len) {
    if (i + 1 >= argc) missing_argument();
    return argv[++i];
  }

  const std::size_t value_len = argument_len - name_len;
  assert(value_len > 0);

  std::ptrdiff_t value_offset = name_len;

  if (argument[name_len] == '=') {
    if (value_len == 1) missing_argument();
    ++value_offset;
  }

  return argument.data() + value_offset;
}

void Config::check_environment_variables() {
  const char* value;

  value = std::getenv(ENV_CONFIG_DIR);
  if (value) config_path = value;
}

void Config::check_arguments(uint32_t argc, const char** argv) {
  for (uint32_t i = 1; i < argc; ++i) {
    const std::string_view argument(argv[i]);

    if (argument == OPTION_HELP) usage();

    const char* value;
    bool matched = false;

    value = get_option_value(OPTION_CONFIG_DIR, i, argc, argv);
    matched |= bool(value);
    if (value) config_path = value;

    if (!matched) {
      fprintf(stderr, ERROR_TAG "Unknown option '%s'\n", argument.data());
      usage();
    }
  }
}

static frc::AprilTagFieldLayout
load_field_layout(const std::filesystem::path& config_file) {
  std::error_code error;
  wpi::raw_fd_istream stream(config_file.c_str(), error);

  const char* filename = config_file.filename().c_str();

  if (error) {
    fprintf(stderr, ERROR_TAG "Failed to open %s file: %s\n", filename,
            error.message().c_str());
    exit(1);
  }

  frc::AprilTagFieldLayout layout;
  try {
    wpi::json json;
    stream >> json;
    layout = json.get<frc::AprilTagFieldLayout>();

  } catch (const wpi::json::exception& e) {
    fprintf(stderr, ERROR_TAG "Failed to parse %s file: %s\n", filename,
            e.what());
    exit(1);
  }

  return layout;
}

Config Config::load(uint32_t argc, const char** agrv) {
  Config config;

  config.check_environment_variables();
  config.check_arguments(argc, agrv);

  if (config.config_path.empty()) {
    fputs(ERROR_TAG "Missing configuration directory", stderr);
    exit(1);
  }
  if (!std::filesystem::exists(config.config_path)) {
    fprintf(stderr, ERROR_TAG "Configuration directory '%s' does not exist\n",
            config.config_path.c_str());
    exit(1);
  }

  config.config_path = std::filesystem::canonical(config.config_path);

  printf(MAIN_TAG "Configuration directory: %s\n", config.config_path.c_str());

  config.vision_config = VisionConfig::load(config.config_path);
  config.networking_config = NetworkingConfig::load(config.config_path);

  const std::filesystem::path field_layout_config_file =
      config.config_path / (config.vision_config.field_layout_name + ".json");

  config.field_layout = load_field_layout(field_layout_config_file);

  return config;
}

