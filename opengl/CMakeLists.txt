cmake_minimum_required(VERSION 3.0.0)
project(openGL VERSION 0.1.0)

include_directories(./include)
#include_directories(./include/GLFW)
#include_directories(./include/KHR)

#add_library(glfw3.dll SHARED .lib/glfw3.dll)
link_libraries(zybk.util)
link_libraries(glfw3.dll)
link_libraries(libglad.a)
link_libraries(libglfw3dll.a)
add_library(zybk.opengl  SHARED ./zybkopengl.cpp ./shader.cpp)
message(${PROJECT_SOURCE_DIR})