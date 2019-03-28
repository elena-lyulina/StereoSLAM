set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_BUILD_TYPE=Release")
set(BENCHMARKS bench_FeatureMatching bench_Frame)

foreach(CUR_BENCH ${BENCHMARKS})
 #   find_package(OpenCV REQUIRED core imgproc highgui)
    add_executable(${CUR_BENCH} ${CUR_BENCH}.cpp)
    target_link_libraries(${CUR_BENCH} benchmark slam)
endforeach(CUR_BENCH)