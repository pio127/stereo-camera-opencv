#include "opencv2/calib3d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>

void cameraTest();
int main() {
  cameraTest();
}


void cameraTest() {
  cv::Mat frame1;
  cv::Mat frame2;
  cv::VideoCapture cap1(0);
  cv::VideoCapture cap2(1);
  cap1.set(CV_CAP_PROP_GAIN,1);
  //cap2.set(CV_CAP_PROP_FPS,30);
  if (cap1.isOpened() && cap2.isOpened()) {
    while (true) {
      cap1.read(frame1);
      cap2.read(frame2);
      cv::imshow("camera1", frame1);
      cv::imshow("camera2", frame2);
      if (cv::waitKey(1) == 27) {
        break;
      }
    }
  }
  cap1.release();
  cap2.release();
}