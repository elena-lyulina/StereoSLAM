set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(SOURCE_FILES src/Frame.cpp src/FeatureDetector.cpp src/FeaturePoint.cpp src/FeatureMatcher.cpp src/FeatureTracker.cpp)
add_library(slam ${SOURCE_FILES})

target_include_directories(slam PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PUBLIC src)

target_link_libraries(slam ${OpenCV_LIBS} )
