cmake_minimum_required(VERSION 3.0.0)
project(VSCode VERSION 0.1.0)

include(CTest)
enable_testing()

set(OUTPATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${OUTPATH})
set(EXECUTABLE_OUTPUT_PATH ${OUTPATH})

include_directories(LIBRARY_OUTPUT_PATH)
include_directories(./leetcode/leetcode)

add_subdirectory(./config)
add_subdirectory(./codingInVIVOTime)
add_subdirectory(./leetcode/leetcode)

add_executable(VSCode main.cpp )



link_directories(LIBRARY_OUTPUT_PATH)
# link_directories(zybk.isvalidbst PUBLIC LIBRARY_OUTPUT_PATH)


target_link_libraries(VSCode test11)
target_link_libraries(VSCode zybk.kmp)
target_link_libraries(VSCode zybk.isvalidbst)


message(${PROJECT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
