#ifndef STEREO_CAPTURE_H
#define STEREO_CAPTURE_H

#include "opencv2/calib3d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

class StereoCapture {
private:
  cv::VideoCapture cap1;
  cv::VideoCapture cap2;
  bool camerasAreReady;
public:
  StereoCapture();
  ~StereoCapture();
  bool readFrames(cv::Mat &frame1, cv::Mat &frame2);
  bool isReady();
};

#endif