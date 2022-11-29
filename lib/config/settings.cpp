#include <RollingRaspberry/config/settings.h>
#include <wpi/raw_istream.h>

Settings::Settings() {
  std::error_code err_code;
  
  wpi::raw_fd_istream cfg_file { THUNDER_CONFIG_JSON_PATH, err_code };
  if (err_code) {
    std::cerr << "Failed to load Config JSON file at " << THUNDER_CONFIG_JSON_PATH << '\n';
  }

  std::cout << "Loaded Config JSON file\n";
  
  wpi::json cfg_json;
  cfg_file >> cfg_json;
  
  {
    // USB Camera list.
    wpi::json usb_cams_json = cfg_json.at("usb_cameras");
    for (const auto& cam_json : usb_cams_json.get<std::vector<wpi::json>>()) {
      int dev = cam_json.at("dev").get<int>();
      bool host = cam_json.at("host_stream").get<bool>();
      CameraProps props = cam_json.at("props").get<CameraProps>();
      
      usb_cameras.emplace_back(dev, props);

      if (host) {
        usb_cameras.back().host_stream();
      }
    }
    
    // MJPG Stream list.
    wpi::json mjpg_cams_json = cfg_json.at("mjpg_cameras");
    for (const auto& cam_json : mjpg_cams_json.get<std::vector<wpi::json>>()) {
      std::string url = cam_json.at("url").get<std::string>();
      CameraProps props = cam_json.at("props").get<CameraProps>();
      
      mjpg_cameras.emplace_back(url, props);
    }
  }
}

Settings::~Settings() = default;
