#include <ThunderCameraCalibrator/calibrator.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

Calibrator::Calibrator(std::vector<std::filesystem::path> _images, Chessboard _chessboard)
: images(_images), chessboard(_chessboard) {
  cv::Size board_size(chessboard.cols, chessboard.rows);
  
  cv::Mat img, gray;
  for (const auto& image : images) {
    // Load the image.
    img = cv::imread(image.string(), cv::IMREAD_COLOR);
    
    // Convert the image to grayscale.
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    
    // Find the chessboard corners.
    bool found = cv::findChessboardCorners(img, board_size, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);
    
    std::vector<cv::Point3f> obj;
    for (double i = 0; i < chessboard.rows; i++) {
      for (double j = 0; j < chessboard.cols; j++) {
        obj.emplace_back(j * chessboard.sq_size.value(), i * chessboard.sq_size.value(), 0);
      }
    }
    
    if (found) {
      // Improve the found corners' coordinate accuracy for chessboard calibration.
      cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));
    
      image_points.push_back(corners);
      object_points.push_back(obj);
      
      std::cout << "Found chessboard in image: " << image << '\n';
    }
    else {
      std::cout << "Failed to find chessboard in image: " << image << '\n';
    }
  }
  
  std::vector<cv::Mat> rvecs, tvecs;
  int flag = 0;
  flag |= cv::CALIB_FIX_K4;
  flag |= cv::CALIB_FIX_K5;
  
  // Calibrate the camera.
  cv::calibrateCamera(object_points, image_points, img.size(), camera_matrix, dist_coeffs, rvecs, tvecs, flag);
}
