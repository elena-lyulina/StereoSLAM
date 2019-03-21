set(TESTS test_Frame test_FeatureDetection)
set(SOURCE_FILES ../src/Frame.cpp)

foreach(CUR_TEST ${TESTS})
    find_package(OpenCV REQUIRED core imgproc highgui)
    add_executable(${CUR_TEST} ${CUR_TEST}.cpp ${SOURCE_FILES})
    target_link_libraries(${CUR_TEST} gtest ${OpenCV_LIBS})
    add_test(${CUR_TEST} ${CUR_TEST})
endforeach(CUR_TEST)