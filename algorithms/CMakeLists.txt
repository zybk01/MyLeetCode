cmake_minimum_required(VERSION 3.0.0)
project(Algorithms VERSION 0.1.0)

message("Enter Path: " ${PROJECT_SOURCE_DIR})

SET(linkLib libopencv_core460.dll libopencv_imgcodecs460.dll libopencv_imgproc460.dll libopencv_objdetect460.dll libopencv_photo460.dll libopencv_highgui460.dll libopencv_features2d460.dll)
# SET(linkSLib libopencv_core460.dll.a libopencv_imgcodecs460.dll.a libopencv_imgproc460.dll.a libopencv_objdetect460.dll.a libopencv_photo460.dll.a libopencv_highgui460.dll.a)
link_libraries(${linkLib})
link_libraries(zybk.util)
add_executable(depthFromDefocus ./defocus/depthFromdefocus.cpp)
add_executable(harris ./harris/harris.cpp)
add_executable(sift ./sift/sift.cpp)


message("Leaving Path: " ${PROJECT_SOURCE_DIR})