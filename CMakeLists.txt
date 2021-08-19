cmake_minimum_required(VERSION 3.0.0)
project(VSCode VERSION 0.1.0)

include(CTest)
enable_testing()

set(OUTPATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${OUTPATH})
set(EXECUTABLE_OUTPUT_PATH ${OUTPATH})

include_directories(./leetcode/leetcode)

add_executable(VSCode main.cpp )
add_subdirectory(./codingInVIVOTime)
add_subdirectory(./leetcode/leetcode)
add_subdirectory(./config)

target_link_libraries(VSCode test11)

message(${PROJECT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
