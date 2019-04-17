set(SOURCE_FILES Frame.cpp Frame.h FeatureDetector.cpp FeatureDetector.h FeaturePoint.cpp FeaturePoint.h FeatureMatcher.h FeatureMatcher.cpp FeatureTracker.cpp FeatureTracker.h util.h)
add_library(slam ${SOURCE_FILES})
target_link_libraries(slam ${OpenCV_LIBS} )
