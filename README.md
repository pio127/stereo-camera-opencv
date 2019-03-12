# Stereo Camera

Using 2 PS3 Eye cameras to get depth map.

Works under Linux(Manjaro, Ubuntu, Raspberry Pi3 running Rasbian) without additional drivers because of ov534 driver coming with kernel.

For Windows it's more tricky to get it working(https://github.com/psmoveservice/PSMoveService/wiki/PSEye-Software-Setup-%28Windows%29).


# Requirements
 - 2 cameras,
 - V4L2 library(if under linux and only for ps eye cameras),
 - built OpenCV or Conan package manager.
 
# Install
```
mkdir build
cd build
conan install ..
cmake ..
cmake --build .
```
Notes:
 - If there is already an OpenCV library installed then conan install is not needed.
 - For problems with dependency packages(such as pre-built packages not found) conan install .. --build=missing should help. It builds them directly from sources.

# Info
There are 3 executables:
 1. testCamera - just image capture and display
 2. calibration - calibrates stereo cam(chessboard pattern board needed), outputs calibration file in yaml format
 3. depthMap - uses calibration file and then generates depth map of captured scene
 
