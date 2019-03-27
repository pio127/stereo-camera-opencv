#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/ximgproc.hpp"
#include "stereoCapture.h"
#include <iostream>
#include <string>
#include <vector>

bool depthMap(std::string);

int main(int argc, char *argv[]) {
	std::string calibrationFile;
	if (argc > 1) {
		calibrationFile = argv[1];	
	} else {
  calibrationFile = "calibration_file.yml";
	}
  depthMap(calibrationFile);
}

bool depthMap(std::string filename) {
  cv::Mat frame1, frame2, frame1Gray, frame2Gray, frame1Remapped,
      frame2Remapped, disp, disp2, disp_norm, disp2_norm;
  cv::Mat imgDisparity32F;
  cv::Mat left_disp2,right_disp2,filtered_disp2, filtered_disp_vis2,
  filtered_disp_vis2_norm,filtered_disp2_vis,left_disp2_norm ;
  cv::Mat CM1, D1, R1, P1, P2, CM2, D2, R2;
  cv::Mat map1x, map1y, map2x, map2y;
  cv::Mat imgU1, imgU2;
  cv::Size size;
  int numDisparities = 5;
  int BlockSize = 6;
  int SpeckleWindowSize = 100;
  int SpeckleRange = 14;
  int MinDisparity = 100;
  int UniquenessRatio = 5;
  int Disp12MaxDiff = 0;
  int PreFilterCap = 22;
  int PreFilterSize = 0;
  int TextureThreshold = 800;
  int lambda=4000;
  int sigma=3;
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
	cv::createTrackbar("lambda", "dispmap", &lambda, 15000);
	cv::createTrackbar("sigma", "dispmap", &sigma, 20);

  cv::initUndistortRectifyMap(CM1, D1, R1, P1, size, CV_16SC2, map1x, map1y);
  cv::initUndistortRectifyMap(CM2, D2, R2, P2, size, CV_16SC2, map2x, map2y);

  cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create();
  cv::Ptr<cv::StereoBM> left_matcher2 = cv::StereoBM::create();
  cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter2 = 
	 cv::ximgproc::createDisparityWLSFilter(left_matcher2);
  cv::Ptr<cv::StereoMatcher> right_matcher2 = 
	 cv::ximgproc::createRightMatcher(left_matcher2);

  while (true) {
    cameras.readFrames(frame1, frame2);

    cv::cvtColor(frame1, frame1Gray, CV_BGR2GRAY);
    cv::cvtColor(frame2, frame2Gray, CV_BGR2GRAY);
    remap(frame1Gray, frame1Remapped, map1x, map1y, cv::INTER_LINEAR,
          cv::BORDER_CONSTANT, cv::Scalar());
    remap(frame2Gray, frame2Remapped, map2x, map2y, cv::INTER_LINEAR,
          cv::BORDER_CONSTANT, cv::Scalar());

   // sbm->setNumDisparities(16 * (numDisparities + 1));
   // sbm->setBlockSize(2 * BlockSize + 5);
   // sbm->setPreFilterCap(PreFilterCap + 1);
   // sbm->setPreFilterSize(2 * PreFilterSize + 5);
   // sbm->setTextureThreshold(TextureThreshold);
   // sbm->setSpeckleWindowSize(SpeckleWindowSize);
   // sbm->setSpeckleRange(SpeckleRange);
   // sbm->setMinDisparity(MinDisparity - 100);
   // sbm->setUniquenessRatio(UniquenessRatio);
   // sbm->setDisp12MaxDiff(Disp12MaxDiff);
   // sbm->compute(frame1Remapped, frame2Remapped, disp);
   // normalize(disp, disp_norm, 0, 255, CV_MINMAX, CV_8U);

	left_matcher2->setNumDisparities(16*(numDisparities+1));
	left_matcher2->setBlockSize(2*BlockSize+5);
	left_matcher2->setPreFilterCap(PreFilterCap+1);
	left_matcher2->setPreFilterSize(2*PreFilterSize+5);
	left_matcher2->setTextureThreshold(TextureThreshold);
	left_matcher2->setSpeckleWindowSize(SpeckleWindowSize);
	left_matcher2->setSpeckleRange(SpeckleRange);
	left_matcher2->setMinDisparity(MinDisparity-100);
	left_matcher2->setUniquenessRatio(UniquenessRatio);
	left_matcher2->setDisp12MaxDiff(Disp12MaxDiff);
	left_matcher2->compute(frame1Remapped, frame2Remapped, disp2);
    disp2.convertTo(imgDisparity32F, CV_32F, 1./16);
	cv::normalize(imgDisparity32F,disp2_norm,0,255,CV_MINMAX,CV_8U);

//	wls_filter2->filter(disp2, frame1Remapped, filtered_disp2, right_disp2);
//	cv::ximgproc::getDisparityVis(filtered_disp2,filtered_disp2_vis);
//	cv::normalize(filtered_disp2_vis, filtered_disp_vis2_norm, 0, 255, CV_MINMAX, CV_8U);
	//cv::namedWindow("filtered SBM", cv::WINDOW_AUTOSIZE);
//	cv::imshow("filtered SBM", filtered_disp_vis2_norm);
    cv::imshow("Kamera Lewa", frame1Remapped);
    cv::imshow("Kamera Prawa", frame2Remapped);
   // cv::imshow("dispmap", disp_norm);
    cv::imshow("dispmap_filtered", disp2_norm);
    if (cv::waitKey(1) == 27) {
      break;
    }
  }
  cv::destroyAllWindows();
  return true;
}
