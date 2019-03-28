cmake_minimum_required(VERSION 2.8.4)
project(StereoSLAM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )

# adding google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/benchmark)

# adding google test
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/googletest)

set(SOURCE_FILES src/main.cpp src/Frame.cpp src/Frame.h src/FeatureDetection.cpp src/FeatureDetection.h src/FeaturePoint.cpp src/FeaturePoint.h)
add_executable(StereoSLAM ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( StereoSLAM ${OpenCV_LIBS} )

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)