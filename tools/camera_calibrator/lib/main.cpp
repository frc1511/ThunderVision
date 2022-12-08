#include <ThunderCameraCalibrator/calibrator.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>

int main(int argc, const char** argv) {
  std::vector<std::string_view> args(argv, argv + argc);

  std::vector<std::filesystem::path> img_paths;

  auto usage = [](bool err = false) {
    auto& stream = err ? std::cerr : std::cout;

    stream << "Usage: camera_calibrator [options] <image directories>\n";
  };

  auto file_exists = [](const std::filesystem::path& path) -> bool {
    struct stat s;
    return stat(path.c_str(), &s) == 0;
  };

  for (decltype(args)::const_iterator it = args.cbegin() + 1; it != args.cend(); ++it) {
    if ((*it).at(0) == '-') {
      if (*it == "-h" || *it == "--help") {
        usage();
        return EXIT_SUCCESS;
      }
      else {
        std::cerr << "Unknown option: " << *it << '\n';
        usage(true);
        return EXIT_FAILURE;
      }
    }
    else {
      std::filesystem::path img_path(*it);
      if (!file_exists(img_path)) {
        std::cerr << "File does not exist: " << img_path.string() << '\n';
        return EXIT_FAILURE;
      }
      img_paths.push_back(img_path);
    }
  }

  if (img_paths.size() < 2) {
    std::cerr << "No image paths specified.\n";
    usage(true);
    return EXIT_FAILURE;
  }
  else if (img_paths.size() < 30) {
    std::cerr << "Warning: less than 30 images specified.\n";
  }

  Calibrator calibrator(img_paths, {9, 6, 0.0254_m});

  std::cout << "Camera matrix:\n" << calibrator.get_camera_matrix() << '\n';
  std::cout << "Distortion coefficients:\n" << calibrator.get_distortion_coefficients() << '\n';
  
  return 0;
}