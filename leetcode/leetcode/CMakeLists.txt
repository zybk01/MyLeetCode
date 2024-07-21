cmake_minimum_required(VERSION 3.0.0)
project(LeetCode VERSION 0.1.0)

message("Enter Path: " ${PROJECT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${OUTPATH}/libs)
set(EXECUTABLE_OUTPUT_PATH ${OUTPATH})
link_libraries(dbghelp.dll kernel32.dll)
if(${BUILD_LOG})
add_library(zybk.util  SHARED zybkLog.cpp zybkTrace.cpp ThreadPool.cpp)
set(CMAKE_CXX_FLAGS "-O0 -g")
endif(${BUILD_LOG})

link_libraries(zybk.util)




if(${BUILD_ZYBKLOG})
# add_library(zybk.log  SHARED zybkLog.h)
endif(${BUILD_ZYBKLOG})

if(${BUILD_THREADPOOL})
# add_library(zybk.threadpool  SHARED ThreadPool.h)
endif(${BUILD_THREADPOOL})

if(${BUILD_ALGORITHMFACTORY})
# add_library(zybk.algorithmfactory  SHARED algorithmFactory.h)
endif(${BUILD_ALGORITHMFACTORY})





if(${BUILD_ALGO_ISBIPARTITE})
add_library(zybk.isbipartite  SHARED isBipartite.cpp)
endif(${BUILD_ALGO_ISBIPARTITE})

if(${BUILD_ALGO_ISVALIDBST})
add_library(zybk.isvalidbst  SHARED isValidBST.cpp)
endif(${BUILD_ALGO_ISVALIDBST})

if(${BUILD_ALGO_RESTOREARRAY})
add_library(zybk.restorearray  SHARED restoreArray.cpp)
endif(${BUILD_ALGO_RESTOREARRAY})

if(${BUILD_ALGO_PERMUTE})
add_library(zybk.permute  SHARED permute.cpp)
endif(${BUILD_ALGO_PERMUTE})

if(${BUILD_ALGO_KMP})
add_library(zybk.kmp  SHARED kmp.cpp)
endif(${BUILD_ALGO_KMP})

if(${BUILD_ALGO_TREE_OF_INFINITESOULS})
add_library(zybk.tressofinfiniteSouls  SHARED treeOfInfiniteSouls.cpp)
endif(${BUILD_ALGO_TREE_OF_INFINITESOULS})

add_library(test11 STATIC test.cpp)
# add_library(zybk.util STATIC ThreadPool.cpp)

link_libraries(zybk.tressofinfiniteSouls)

if(${BUILD_LEETCODE})

#set(LIBRARY_OUTPUT_DIRECTORY ${OUTPATH})
add_executable(LeetCode leetcode.cpp)
# target_link_libraries(VSCode test11)
message("BUILDING_LEETCODE_DONE: "${BUILD_LEETCODE})
endif()


message("Leaving Path: " ${PROJECT_SOURCE_DIR})