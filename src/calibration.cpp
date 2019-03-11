#include "opencv2/calib3d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>

bool calibrate(std::string, int);

int main() { calibrate("kalibracja", 20); }

bool calibrate(std::string filename, int numOfFrames) {
  int count = 0;
  std::vector<std::vector<cv::Point3f>> objectPoints;
  std::vector<std::vector<cv::Point2f>> imagePoints1, imagePoints2;

  std::vector<cv::Point2f> points1;
  std::vector<cv::Point2f> points2;
  cv::Mat L;
  cv::Mat P;
  cv::Mat Lf;
  cv::Mat Pf;
  std::vector<cv::Point3f> obj;

  cv::VideoCapture cap1(0);
  cv::VideoCapture cap2(1);

  if (cap1.isOpened() == false) {
    std::cerr << "Camera 1 did not start\n";
    return false;
  }
  if (cap2.isOpened() == false) {
    std::cerr << "Camera 2 did not start\n";
  }

  // Chessboard pattern generation
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 6; j++) {
      obj.push_back(cv::Point3f((float)j * 2.6, (float)i * 2.6, 0.0f));
    }
  }

  while (true) {
    cap1.read(L);
    cap2.read(P);

    bool found_L = cv::findChessboardCorners(L, cv::Size(6, 9), points1,
                                             CV_CALIB_CB_ADAPTIVE_THRESH |
                                                 CV_CALIB_CB_NORMALIZE_IMAGE);
    L.copyTo(Lf);
    drawChessboardCorners(Lf, cv::Size(6, 9), points1, found_L);

    bool found_P = cv::findChessboardCorners(P, cv::Size(6, 9), points2,
                                             CV_CALIB_CB_ADAPTIVE_THRESH |
                                                 CV_CALIB_CB_NORMALIZE_IMAGE);
    P.copyTo(Pf);
    drawChessboardCorners(Pf, cv::Size(6, 9), points2, found_P);

    if (found_L)
      cv::imshow("Kamera Lewa", Lf);
    else
      cv::imshow("Kamera Lewa", L);
    if (found_P)
      cv::imshow("Kamera Prawa", Pf);
    else
      cv::imshow("Kamera Prawa", P);

    // if (found_L && found_P)
    //{
    //  std::stringstream l_name, r_name;
    //  l_name << "img\\left" << std::setw(2) << std::setfill('0') << count <<
    //  ".jpg"; r_name << "img\\right" << std::setw(2) << std::setfill('0') <<
    //  count << ".jpg"; imwrite(l_name.str(), L); imwrite(r_name.str(), P);
    //  std::cout << "Zapisono obrazy " << count << std::endl;
    //  count++;
    //}

    if (found_L && found_P) {
      imagePoints1.push_back(points1);
      imagePoints2.push_back(points2);
      objectPoints.push_back(obj);
      std::cout << "Zebrano pomiar " << count << std::endl;
      count++;
    }
    if (count == numOfFrames || cv::waitKey(1) == 27) {
      break;
    }
  }
    // destroyAllWindows();
    if (count == numOfFrames) {
      std::cout << "Kalibracja..." << std::endl;
      cv::Mat CM1 = cv::Mat(3, 3, CV_64FC1);
      cv::Mat CM2 = cv::Mat(3, 3, CV_64FC1);
      cv::Mat D1, D2;
      cv::Mat R, T, E, F;

      cv::stereoCalibrate(
          objectPoints, imagePoints1, imagePoints2, CM1, D1, CM2, D2, L.size(),
          R, T, E, F, cv::CALIB_FIX_INTRINSIC,
          cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));

      cv::Mat R1, R2, P1, P2, Q;
      cv::stereoRectify(CM1, D1, CM2, D2, L.size(), R, T, R1, R2, P1, P2, Q);

      cv::FileStorage fs(filename + ".yml", cv::FileStorage::WRITE);
      fs << "CM1" << CM1;
      fs << "CM2" << CM2;
      fs << "D1" << D1;
      fs << "D2" << D2;
      fs << "R" << R;
      fs << "T" << T;
      fs << "E" << E;
      fs << "F" << F;
      fs << "R1" << R1;
      fs << "R2" << R2;
      fs << "P1" << P1;
      fs << "P2" << P2;
      fs << "Q" << Q;
      fs << "Size" << L.size();
      std::cout << "Calibration complete" << std::endl;
    }
    cap1.release();
    cap2.release();
    return true;
}