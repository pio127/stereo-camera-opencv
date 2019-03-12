#include "stereoCapture.h"
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
  StereoCapture cameras{};
  //cap1.set(CV_CAP_PROP_GAIN,1);
  //cap2.set(CV_CAP_PROP_FPS,30);
  if (cameras.areReady()) {
    while (true) {
      cameras.readFrames(frame1, frame2);
      cv::imshow("camera1", frame1);
      cv::imshow("camera2", frame2);
      if (cv::waitKey(1) == 27) {
        break;
      }
    }
  }
}