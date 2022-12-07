#include <RollingRaspberry/vision/apriltag_detector.h>

AprilTagDetector::AprilTagDetector(AprilTagDetectorSettings settings)
: tag_family(tag16h5_create()), tag_detector(apriltag_detector_create()) {
  apriltag_detector_add_family(tag_detector, tag_family);

  fmt::print("Using {} threads\n", tag_detector->nthreads);
}

AprilTagDetector::~AprilTagDetector() {
  apriltag_detector_destroy(tag_detector);
  tag16h5_destroy(tag_family);
}

std::vector<AprilTagDetector::Detection> AprilTagDetector::detect(cv::Mat& frame) {
  image_u8_t im = { frame.cols, frame.rows, frame.cols, frame.data };
  
  zarray_t* detections = apriltag_detector_detect(tag_detector, &im);
  
  std::vector<Detection> result;
  
  for (int i = 0; i < zarray_size(detections); i++) {
    apriltag_detection_t* det;
    zarray_get(detections, i, &det);
    
    // Reduce false positives by requiring a minimum decision margin.
    if (det->decision_margin < settings.min_decision_margin) continue;
    
    Eigen::Matrix<double, 3, 3> homography {
      { matd_get(det->H, 0, 0), matd_get(det->H, 0, 1), matd_get(det->H, 0, 2) },
      { matd_get(det->H, 1, 0), matd_get(det->H, 1, 1), matd_get(det->H, 1, 2) },
      { matd_get(det->H, 2, 0), matd_get(det->H, 2, 1), matd_get(det->H, 2, 2) }
    };
    
    cv::Point2d center { det->c[0], det->c[1] };
    
    std::array<cv::Point2d, 4> corners {
      cv::Point2d { det->p[0][0], det->p[0][1] },
      cv::Point2d { det->p[1][0], det->p[1][1] },
      cv::Point2d { det->p[2][0], det->p[2][1] },
      cv::Point2d { det->p[3][0], det->p[3][1] }
    };
    
    result.push_back({ det->id, det->hamming, det->decision_margin, homography, center, corners });
  }
  
  apriltag_detections_destroy(detections);
  
  return result;
}

void AprilTagDetector::visualize_detection(cv::Mat& frame, Detection& det) {
  cv::line(frame, det.corners[2], det.corners[3], cv::Scalar(0, 255, 255), 2); // Top
  cv::line(frame, det.corners[0], det.corners[1], cv::Scalar(0, 255, 0),   2); // Bottom
  cv::line(frame, det.corners[1], det.corners[2], cv::Scalar(255, 0, 0),   2); // Left
  cv::line(frame, det.corners[3], det.corners[0], cv::Scalar(0, 0, 255),   2); // Right
  
  std::string text = std::to_string(det.id);
  int font_face = cv::FONT_HERSHEY_SIMPLEX;
  double font_scale = 1.0;
  int base_line;
  cv::Size text_size = cv::getTextSize(text, font_face, font_scale, 2, &base_line);
  cv::putText(frame, text, cv::Point(det.center.x - text_size.width/2, det.center.y + text_size.height/2), font_face, font_scale, cv::Scalar(255, 153, 0), 2);
}
