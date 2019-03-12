#include "opencv2/calib3d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>

bool depthMap(std::string);

int main() { depthMap("kalibracja.yaml"); }

bool depthMap(std::string filename) {
  cv::Mat L, P, LG, PG, L2, P2, disp, disp2;
  cv::Mat CM1, D1, R1, P1, CM2, D2, R2;
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

  cv::VideoCapture cap1(0);
  cv::VideoCapture cap2(1);

  if (cap1.isOpened() == false) {
    std::cerr << "Camera 1 did not start\n";
    return false;
  }
  if (cap2.isOpened() == false) {
    std::cerr << "Camera 2 did not start\n";
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

  cv::initUndistortRectifyMap(CM1, D1, R1, P1, size, CV_32FC1, map1x, map1y);
  cv::initUndistortRectifyMap(CM2, D2, R2, P2, size, CV_32FC1, map2x, map2y);

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

  while (true) {
    cap1.read(L);
    cap2.read(P);

    cv::cvtColor(L, LG, CV_BGR2GRAY);
    cv::cvtColor(P, PG, CV_BGR2GRAY);
    remap(LG, L2, map1x, map1y, cv::INTER_LINEAR, cv::BORDER_CONSTANT,
          cv::Scalar());
    remap(PG, P2, map2x, map2y, cv::INTER_LINEAR, cv::BORDER_CONSTANT,
          cv::Scalar());

    sbm->compute(L2, P2, disp);
    normalize(disp, disp2, 0, 255, CV_MINMAX, CV_8U);

    imshow("Kamera Lewa", L2);
    imshow("Kamera Prawa", P2);
    imshow("disp", disp2);
  }
  return true;
}
