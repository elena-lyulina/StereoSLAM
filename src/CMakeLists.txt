include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(slam)
add_executable(StereoSLAM main.cpp)
target_link_libraries(StereoSLAM slam)