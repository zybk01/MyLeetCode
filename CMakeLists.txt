cmake_minimum_required(VERSION 3.0.0)
project(VSCode VERSION 0.1.0)

include(CTest)
enable_testing()

add_definitions(-DDUMPPATH="D:\\\\repo\\\\MyLeetCode\\\\backtrace.txt")
add_definitions(-DDEBUG_ENABLE=1)

set(OUTPATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${OUTPATH}/libs)
set(EXECUTABLE_OUTPUT_PATH ${OUTPATH})
link_directories(LIBRARY_OUTPUT_PATH)
link_directories(./opengl/lib)

include_directories(LIBRARY_OUTPUT_PATH)
include_directories(./leetcode/leetcode)
include_directories(./opengl)

if(0)
add_subdirectory(./codingInVIVOTime)
endif()


if(1)

add_subdirectory(./config)

add_subdirectory(./leetcode/leetcode)

add_subdirectory(./opengl)

link_libraries(zybk.util)
link_libraries(zybk.opengl)

add_executable(VSCode main.cpp )
# add_compile_definitions(DUMPPATH ${PROJECT_SOURCE_DIR})


target_link_libraries(VSCode test11)
target_link_libraries(VSCode zybk.kmp)
target_link_libraries(VSCode zybk.isvalidbst)

endif()

# link_directories(zybk.isvalidbst PUBLIC LIBRARY_OUTPUT_PATH)




message(${PROJECT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
