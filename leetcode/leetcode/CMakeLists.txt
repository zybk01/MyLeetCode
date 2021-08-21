cmake_minimum_required(VERSION 3.0.0)
project(LeetCode VERSION 0.1.0)

if(1)
    #set(LIBRARY_OUTPUT_DIRECTORY ${OUTPATH})
    set(LIBRARY_OUTPUT_PATH ${OUTPATH})
    set(EXECUTABLE_OUTPUT_PATH ${OUTPATH})

    add_executable(LeetCode leetcode.cpp)

    add_library(zybk.kmp  SHARED kmp.cpp)
    
    
endif()

add_library(test11 STATIC test.cpp)
add_library(zybk.util STATIC ThreadPool.cpp)


message(${PROJECT_SOURCE_DIR})