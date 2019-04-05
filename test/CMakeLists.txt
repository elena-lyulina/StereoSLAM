set(TESTS test_Frame test_FeatureDetection)

foreach(CUR_TEST ${TESTS})
    find_package(OpenCV REQUIRED core imgproc highgui)
    add_executable(${CUR_TEST} ${CUR_TEST}.cpp)
    target_link_libraries(${CUR_TEST} gtest ${OpenCV_LIBS} slam)
    add_test(${CUR_TEST} ${CUR_TEST})
endforeach(CUR_TEST)