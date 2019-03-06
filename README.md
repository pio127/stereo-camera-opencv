### Stereo Camera

Using 2 PS3 Eye cameras to get depth map.
Works under Linux(Manjaro, Ubuntu) without additional drivers(only V4L2 needed).

### Requirements
 - 2 cameras,
 - V4L2 library(if under linux and only for ps eye cameras),
 - built OpenCV or Conan package manager.
 
### Install
```
mkdir build
cd build
conan install ..
cmake ..
cmake --build .
```

### Info
There are 3 executables:
 1. testCamera - just image capture and display
 2. calibration - calibrates stereo cam(chessboard pattern board needed), outputs calibration file in yaml format
 3. depthMap - uses calibration file and then generates depth map of captured scene
 
