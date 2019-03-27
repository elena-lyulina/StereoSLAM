set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_BUILD_TYPE=Release")
set(BENCHMARKS bench_First)
set(SOURCE_FILES ../src/Frame.cpp ../src/FeatureDetection.cpp ../src/FeaturePoint.cpp)

foreach(CUR_BENCH ${BENCHMARKS})
    find_package(OpenCV REQUIRED core imgproc highgui)
    add_executable(${CUR_BENCH} ${CUR_BENCH}.cpp ${SOURCE_FILES})
    target_link_libraries(${CUR_BENCH} benchmark ${OpenCV_LIBS})
endforeach(CUR_BENCH)