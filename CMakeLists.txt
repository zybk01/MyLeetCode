cmake_minimum_required(VERSION 3.0.0)
project(VSCode VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(VSCode main.cpp)
add_subdirectory(./codingInVIVOTime)
add_subdirectory(./leetcode/leetcode)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
