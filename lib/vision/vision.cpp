#include <RollingRaspberry/vision/vision.h>

Vision::Vision(Settings& settings) 
: usb_cameras(settings.get_usb_cameras()),
  mjpg_cameras(settings.get_mjpg_cameras()),
  tag_family(tag16h5_create()),
  tag_detector(apriltag_detector_create()) {
  
  apriltag_detector_add_family(tag_detector, tag_family);
}

Vision::~Vision() {

}

void Vision::reset_to_mode(NTHandler::MatchMode mode) {

}

void Vision::process() {

}

