cmake_minimum_required(VERSION 3.0.0)
project(VIVOTime VERSION 0.1.0)


add_executable(VIVOTime vivoTime.cpp)
add_library(zybk.log  SHARED ../leetcode/leetcode/zybkLog.cpp)
target_link_libraries(VIVOTime zybk.log)
message(${PROJECT_SOURCE_DIR})