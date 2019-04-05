cmake_minimum_required(VERSION 2.8.4)
project(StereoSLAM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)


# adding google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/benchmark)
add_subdirectory(bench)


# adding google test
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/googletest)
enable_testing()
add_subdirectory(test)

