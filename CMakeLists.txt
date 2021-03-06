cmake_minimum_required(VERSION 2.8.4)
project(StereoSLAM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${Cmake_CXX_FLAGS} -march=native -O3 -DNDEBUG" )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

# adding glog
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Glog REQUIRED)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# adding google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/benchmark)
add_subdirectory(bench)


# adding google test
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/googletest)
enable_testing()
add_subdirectory(test)


add_subdirectory(src)


