cmake_minimum_required(VERSION 2.8.4)
project(StereoSLAM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )
find_package(Eigen3 REQUIRED)


# adding google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/benchmark)

# adding google test
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/googletest)

set(SOURCE_FILES src/Frame.cpp src/Frame.h src/SOFT_FeatureDetector.cpp src/SOFT_FeatureDetector.h src/FeaturePoint.cpp src/FeaturePoint.h)
add_library(slam ${SOURCE_FILES})


add_executable(StereoSLAM src/main.cpp)
target_link_libraries(StereoSLAM slam)

#include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(slam ${OpenCV_LIBS} )

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)