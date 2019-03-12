#include "stereoCapture.h"

StereoCapture::StereoCapture() {
  cap1.open(0);
  cap2.open(1);
  if (!cap1.isOpened()) {
    std::cerr << "Camera /dev/video0 could not be opened\n";
    camerasAreReady = false;
  }
  if (!cap2.isOpened()) {
    std::cerr << "Camera /dev/video1 could not be opened\n";
    camerasAreReady = false;
  }
  camerasAreReady = true;
}

StereoCapture::~StereoCapture() {
  cap1.release();
  cap2.release();
}

bool StereoCapture::isReady() { return camerasAreReady; }

bool StereoCapture::readFrames(cv::Mat &frame1, cv::Mat &frame2) {
  // Copy both frames to internal buffer first and then decoding
  if (cap1.grab() && cap2.grab()) {
    cap1.retrieve(frame1);
    cap2.retrieve(frame2);
    return true;
  } else {
    std::cerr << "Could not copy both frames to internal buffer\n"; 
    return false;
  }
}