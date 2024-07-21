cmake_minimum_required(VERSION 3.0.0)
project(openGL VERSION 0.1.0)

include_directories(./include)

SET(linkLib libopencv_core460.dll libopencv_imgcodecs460.dll libopencv_imgproc460.dll libopencv_objdetect460.dll libopencv_photo460.dll libopencv_highgui460.dll)
# SET(linkSLib libopencv_core460.dll.a libopencv_imgcodecs460.dll.a libopencv_imgproc460.dll.a libopencv_objdetect460.dll.a libopencv_photo460.dll.a libopencv_highgui460.dll.a)
link_libraries(${linkLib})
link_libraries(zybk.util)
link_libraries(glfw3.dll)
link_libraries(libglad.a)
link_libraries(libglfw3dll.a)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O0 -g")

add_library(zybk.opengl  SHARED ./zybkopengl.cpp ./shader.cpp)
message(${PROJECT_SOURCE_DIR})