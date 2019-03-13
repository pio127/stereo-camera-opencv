#include "opencv2/opencv.hpp"
#include "stereoCapture.h"
#include <iostream>
#include <string>
#include <vector>

bool depthMap(std::string);

int main() {
  std::string workingDirectory = "/home/piotr/Projects/stereoCam/build/bin/";
  depthMap(workingDirectory + "calibration_file.yml");
}

bool depthMap(std::string filename) {
  cv::Mat frame1, frame2, frame1Gray, frame2Gray, frame1Remapped,
      frame2Remapped, disp, disp2;
  cv::Mat CM1, D1, R1, P1, P2, CM2, D2, R2;
  cv::Mat map1x, map1y, map2x, map2y;
  cv::Mat imgU1, imgU2;
  cv::Size size;
  int numDisparities = 0;
  int BlockSize = 7;
  int SpeckleWindowSize = 400;
  int SpeckleRange = 49;
  int MinDisparity = 100;
  int UniquenessRatio = 0;
  int Disp12MaxDiff = 0;
  int PreFilterCap = 22;
  int PreFilterSize = 0;
  int TextureThreshold = 1800;

  StereoCapture cameras{};
  if (!cameras.areReady()) {
    return 1;
  }
  cv::FileStorage fs(filename, cv::FileStorage::READ);
  if (!fs.isOpened()) {
    std::cerr << "Calibration file not found\n";
    return false;
  }
  fs["CM1"] >> CM1;
  fs["CM2"] >> CM2;
  fs["D1"] >> D1;
  fs["D2"] >> D2;
  fs["R1"] >> R1;
  fs["R2"] >> R2;
  fs["P1"] >> P1;
  fs["P2"] >> P2;
  fs["Size"] >> size;

  cv::namedWindow("dispmap");
  cv::createTrackbar("numDisparities*16", "dispmap", &numDisparities, 20);
  cv::createTrackbar("BlockSize*2", "dispmap", &BlockSize, 100);
  cv::createTrackbar("SpeckleWindowSize", "dispmap", &SpeckleWindowSize, 1000);
  cv::createTrackbar("SpeckleRange", "dispmap", &SpeckleRange, 31);
  cv::createTrackbar("MinDisparity-100", "dispmap", &MinDisparity, 200);
  cv::createTrackbar("UniquenessRatio", "dispmap", &UniquenessRatio, 100);
  cv::createTrackbar("Disp12MaxDiff", "dispmap", &Disp12MaxDiff, 255);
  cv::createTrackbar("PreFilterCap", "dispmap", &PreFilterCap, 62);
  cv::createTrackbar("PreFilterSize", "dispmap", &PreFilterSize, 20);
  cv::createTrackbar("TextureThreshold", "dispmap", &TextureThreshold, 4000);


  cv::initUndistortRectifyMap(CM1, D1, R1, P1, size, CV_16SC2, map1x, map1y);
  cv::initUndistortRectifyMap(CM2, D2, R2, P2, size, CV_16SC2, map2x, map2y);

  cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create();
  // sbm.state->SADWindowSize = 9;
  // sbm.state->numberOfDisparities = 112;
  // sbm.state->preFilterSize = 5;
  // sbm.state->preFilterCap = 61;
  // sbm.state->minDisparity = -39;
  // sbm.state->textureThreshold = 507;
  // sbm.state->uniquenessRatio = 0;
  // sbm.state->speckleWindowSize = 0;
  // sbm.state->speckleRange = 8;
  // sbm.state->disp12MaxDiff = 1;

  while (true) {
    cameras.readFrames(frame1, frame2);

    cv::cvtColor(frame1, frame1Gray, CV_BGR2GRAY);
    cv::cvtColor(frame2, frame2Gray, CV_BGR2GRAY);
    remap(frame1Gray, frame1Remapped, map1x, map1y, cv::INTER_LINEAR,
          cv::BORDER_CONSTANT, cv::Scalar());
    remap(frame2Gray, frame2Remapped, map2x, map2y, cv::INTER_LINEAR,
          cv::BORDER_CONSTANT, cv::Scalar());

    sbm->setNumDisparities(16 * (numDisparities + 1));
    sbm->setBlockSize(2 * BlockSize + 5);
    sbm->setPreFilterCap(PreFilterCap + 1);
    sbm->setPreFilterSize(2 * PreFilterSize + 5);
    sbm->setTextureThreshold(TextureThreshold);
    sbm->setSpeckleWindowSize(SpeckleWindowSize);
    sbm->setSpeckleRange(SpeckleRange);
    sbm->setMinDisparity(MinDisparity - 100);
    sbm->setUniquenessRatio(UniquenessRatio);
    sbm->setDisp12MaxDiff(Disp12MaxDiff);
    sbm->compute(frame1Remapped, frame2Remapped, disp);
    normalize(disp, disp2, 0, 255, CV_MINMAX, CV_8U);

    cv::imshow("Kamera Lewa", frame1Remapped);
    cv::imshow("Kamera Prawa", frame2Remapped);
    cv::imshow("dispmap", disp2);
    if (cv::waitKey(1) == 27) {
      break;
    }
  }
  cv::destroyAllWindows();
  return true;
}
