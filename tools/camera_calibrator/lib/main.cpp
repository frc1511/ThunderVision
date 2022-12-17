#include <ThunderCameraCalibrator/calibrator.h>
#include <units/length.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <cstdlib>

const char* short_opts = "hs:r:c:";

const struct option long_opts[] = {
  {"help",        no_argument,       nullptr, 'h'},
  {"square_size", required_argument, nullptr, 's'},
  {"rows",        required_argument, nullptr, 'r'},
  {"columns",     required_argument, nullptr, 'c'},
  { nullptr,      0,                 nullptr,  0 }
};

int main(int argc, char* const* argv) {
  std::vector<std::filesystem::path> img_paths;
  
  auto usage = [](bool err = false) {
    auto& stream = err ? std::cerr : std::cout;
    
    stream << "Usage: camera_calibrator [options] <image directories>\n";
    
    stream << "Options:\n";
    stream << "\t--help\t\tShows usage (-h)\n";
    stream << "\t--rows\t\tChessboard rows (-r)\n";
    stream << "\t--columns\tChessboard columns (-c)\n";
    stream << "\t--square_size\tChessboard square size (cm) (-s)\n";
  };
  
  Chessboard chessboard { -1, -1, -1_cm };
  
  int opt, i;
  while ((opt = getopt_long_only(argc, argv, short_opts, long_opts, &i)) != -1) {
    switch (opt) {
      case 'h':
        usage();
        return EXIT_SUCCESS;
      case 's':
        chessboard.sq_size = units::centimeter_t(std::atof(optarg));
        if (chessboard.sq_size <= 0_cm) {
          std::cerr << "Square size must be greater than 0 cm\n";
          return EXIT_FAILURE;
        }
        break;
      case 'r':
        chessboard.rows = std::atoi(optarg);
        if (chessboard.rows <= 0) {
          std::cerr << "Number of rows must be greater than 0\n";
          return EXIT_FAILURE;
        }
        break;
      case 'c':
        chessboard.cols = std::atoi(optarg);
        if (chessboard.cols <= 0) {
          std::cerr << "Number of columns must be greater than 0\n";
          return EXIT_FAILURE;
        }
        break;
      default:
        usage(true);
        return EXIT_FAILURE;
    }
  }
  
  if (chessboard.sq_size == -1_cm || chessboard.rows == -1 || chessboard.cols == -1) {
    std::cerr << "Chessboard rows, columns, and square size must be specified\n";
    return EXIT_FAILURE;
  }
  
  auto file_exists = [](const std::filesystem::path& path) -> bool {
    struct stat s;
    return stat(path.c_str(), &s) == 0;
  };
  
  while (optind < argc) {
    std::filesystem::path img_path(argv[optind++]);
    if (!file_exists(img_path)) {
      std::cerr << "Image file '" << img_path.string() << "' does not exist\n";
      return EXIT_FAILURE;
    }
    img_paths.emplace_back(img_path);
  }
  
  if (img_paths.size() < 2) {
    std::cerr << "No image paths specified.\n";
    return EXIT_FAILURE;
  }
  
  Calibrator calibrator(img_paths, chessboard);
  
  std::cout << "Camera matrix:\n" << calibrator.get_camera_matrix() << '\n';
  std::cout << "Distortion coefficients:\n" << calibrator.get_distortion_coefficients() << '\n';
  
  return 0;
}
