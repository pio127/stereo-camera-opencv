#include "stereoCapture.h"
#include <string>
#include <sys/stat.h>
#include <vector>
#include <filesystem>

// bool calibrate(std::string, int);
bool gatherFramesForCalibration(std::string folderName, int numOfFrames);
bool calibrate(std::string imagesFolderName, std::string calibrationFileName,
               int numOfFrames);

int main(int argc, char *argv[]) {
	
	std::string calibImagesFolder;
	if (argc > 1) {
	  calibImagesFolder = argv[1];
		std::cout << "Using folder: " << calibImagesFolder << '\n';
	} else {	
 	  calibImagesFolder = "calibration_images";
		std::cout << "Using default folder: " << calibImagesFolder << '\n';
  }
	gatherFramesForCalibration(calibImagesFolder, 20);

  //std::filesystem::path workingPath = std::filesystem::current_path();
  //std::string workingPath_str = workingPath.string();
  //std::cout << workingPath_str << "\n";

  calibrate(calibImagesFolder, "calibration_file", 20);
}

bool gatherFramesForCalibration(std::string folderName, int numOfFrames) {
  int count = 0;

  std::vector<cv::Point2f> points1;
  std::vector<cv::Point2f> points2;
  std::vector<cv::Point3f> obj;
  cv::Mat frame1, frame2;
  cv::Mat frame1ForSave, frame2ForSave;
  bool found1, found2;
  StereoCapture cameras{};


  if (!cameras.areReady()) {
    return 1;
  }

  if (!mkdir(folderName.c_str(), 0777)) {
    std::cout << "Folder " << folderName << " created\n";
  } else {
    std::cout << "Folder " << folderName << " already exists\n";
  }

  // Chessboard pattern generation
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 6; j++) {
      obj.push_back(cv::Point3f((float)j * 2.6, (float)i * 2.6, 0.0f));
    }
  }

  while (true) {
    int key{};
    cameras.readFrames(frame1, frame2);

    found1 = cv::findChessboardCorners(frame1, cv::Size(6, 9), points1,
                                       CV_CALIB_CB_ADAPTIVE_THRESH |
                                           CV_CALIB_CB_FAST_CHECK);

    found2 = cv::findChessboardCorners(frame2, cv::Size(6, 9), points2,
                                       CV_CALIB_CB_ADAPTIVE_THRESH |
                                           CV_CALIB_CB_FAST_CHECK);
    if (found1) {
      frame1.copyTo(frame1ForSave);
      drawChessboardCorners(frame1, cv::Size(6, 9), points1, found1);
    }
    if (found2) {
      frame2.copyTo(frame2ForSave);
      drawChessboardCorners(frame2, cv::Size(6, 9), points2, found2);
    }

    cv::imshow("Kamera 1", frame1);
    cv::imshow("Kamera 2", frame2);
    key = cv::waitKey(1);

    if (found1 && found2 && key == 32) {
      std::string nameImage1, nameImage2;
      nameImage1 = folderName + "/" + std::to_string(count) + "_0.jpg";
      nameImage2 = folderName + "/" + std::to_string(count) + "_1.jpg";

      imwrite(nameImage1, frame1ForSave);
      imwrite(nameImage2, frame2ForSave);
      std::cout << "Zapisono obrazy " << count << std::endl;
      count++;
    }
    if (count == numOfFrames || key == 27) {
      break;
    }
  }
  cv::destroyAllWindows();
  return true;
}
bool calibrate(std::string imagesFolderName, std::string calibrationFileName,
               int numOfFrames) {

  cv::Mat frame1, frame2;
  std::string nameImage1, nameImage2;
  int count{0};
  bool found1, found2;
  std::vector<cv::Point2f> points1{};
  std::vector<cv::Point2f> points2{};
  std::vector<cv::Point3f> obj{};
  std::vector<std::vector<cv::Point3f>> calibrationPatternPoints{};
  std::vector<std::vector<cv::Point2f>> imagePoints1{}, imagePoints2{};
  cv::Mat CM1, CM2;
  cv::Mat D1, D2;
  cv::Mat R, T, E, F;
  cv::Mat R1, R2, P1, P2, Q;
  cv::Size frameDimensions{};

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 6; j++) {
      obj.push_back(cv::Point3f((float)j * 2.6, (float)i * 2.6, 0.0f));
    }
  }

  while (true) {
    nameImage1 = imagesFolderName + "/" + std::to_string(count) + "_0.jpg";
    nameImage2 = imagesFolderName + "/" + std::to_string(count) + "_1.jpg";

    frame1 = cv::imread(nameImage1);
    frame2 = cv::imread(nameImage2);
    
    if (!frame1.empty() && frameDimensions.empty()) {
      frameDimensions = frame1.size();
    }

    if (frame1.empty() || frame2.empty()) {
      break;
    } else {
      std::cout << "Wczytano obraz " << count << '\n';
    }

    found1 = cv::findChessboardCorners(frame1, cv::Size(6, 9), points1,
                                       CV_CALIB_CB_ADAPTIVE_THRESH |
                                           CV_CALIB_CB_FAST_CHECK);

    found2 = cv::findChessboardCorners(frame2, cv::Size(6, 9), points2,
                                       CV_CALIB_CB_ADAPTIVE_THRESH |
                                           CV_CALIB_CB_FAST_CHECK);
    if (found1 && found2) {
      imagePoints1.push_back(points1);
      imagePoints2.push_back(points2);
      calibrationPatternPoints.push_back(obj);
    }
    count++;
  }

	if (count == 0) {
		std::cout << "Error: could not read images from folder: " << imagesFolderName << '\n';
	return false;
}
  std::cout << "Kalibracja..." << std::endl;

  cv::stereoCalibrate(
      calibrationPatternPoints, imagePoints1, imagePoints2, CM1, D1, CM2, D2,
      frameDimensions, R, T, E, F, CV_CALIB_ZERO_TANGENT_DIST,
      cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));

  std::cout << CM1;
  cv::stereoRectify(CM1, D1, CM2, D2, frameDimensions, R, T, R1, R2, P1, P2, Q);

  cv::FileStorage fs(calibrationFileName + ".yml", cv::FileStorage::WRITE);
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
  fs << "Size" << frameDimensions;
  std::cout << "Kalibracja zakonczona" << std::endl;

  return true;
}
