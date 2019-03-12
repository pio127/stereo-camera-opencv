#include "stereoCapture.h"

StereoCapture::StereoCapture() {
  int repeatTest = 5;
  int afterTrying = 0;
  for(int x = 0; x < repeatTest; ++x) {
    afterTrying = x;
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

    // Test by capturing image from both cameras
    cv::Mat frame1, frame2;
    if (!cap1.read(frame1) || !cap2.read(frame2)) {
      std::cerr << "Cold not grab both frames\n";
    } else {
      camerasAreReady = true;
      break;
    }
  }
  std::cout << "After trying " << afterTrying+1 << " times:\n";
  if (camerasAreReady) {
    std::cout << "Both cameras are working\n";
  } else {
    std::cerr << "Both cameras are not working\n";
  }
}

StereoCapture::~StereoCapture() {
  cap1.release();
  cap2.release();
}

bool StereoCapture::areReady() { return camerasAreReady; }

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