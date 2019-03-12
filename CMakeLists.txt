cmake_minimum_required(VERSION 3.10)
project(OpenCVProject)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV QUIET)

if(OpenCV_FOUND)
    message("OpenCV FOUND")
    include_directories(include ${OpenCV_INCLUDE_DIRS} src)
    add_library(processing SHARED 
                src/stereoCapture.cpp
                src/stereoCapture.h)
    add_executable(cameraTest src/cameraTest.cpp)
    add_executable(calibration src/calibration.cpp)
    add_executable(depthMap src/depthMap.cpp)
    target_link_libraries(cameraTest ${OpenCV_LIBS} processing)
    target_link_libraries(calibration ${OpenCV_LIBS} processing)
    target_link_libraries(depthMap ${OpenCV_LIBS} processing)
else()
    message("OpenCV NOT FOUND")
    message("Trying to find OpenCV library via Conan Package Manager")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    include_directories(include src)
    add_library(processing SHARED 
                src/stereoCapture.cpp
                src/stereoCapture.h)
    add_executable(cameraTest src/cameraTest.cpp)
    add_executable(calibration src/calibration.cpp)
    add_executable(depthMap src/depthMap.cpp)
    target_link_libraries(cameraTest ${CONAN_LIBS} processing)
    target_link_libraries(calibration ${CONAN_LIBS} processing)
    target_link_libraries(depthMap ${CONAN_LIBS} processing)
endif()    
