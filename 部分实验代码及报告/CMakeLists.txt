cmake_minimum_required(VERSION 3.30)
project(JK160510)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23)

#'cblas_sgemm' is deprecated: first deprecated in macOS 13.3 - An updated CBLAS interface supporting ILP64 is available.
#compile with -DACCELERATE_NEW_LAPACK to access the new headers
add_compile_definitions(ACCELERATE_NEW_LAPACK)
#and -DACCELERATE_LAPACK_ILP64 for ILP64 support
add_compile_definitions(ACCELERATE_LAPACK_ILP64)

# Add MPI Package to Project
find_package(MPI REQUIRED)
# 查找 OpenCL 包（可选步骤，用于跨平台支持）
find_package(OpenCL REQUIRED)

add_executable(JK160510
        exp1_1.cpp
        openmpi_test.cpp
        exp1_2.cpp
        exp2_1.cpp
        exp2_2.cpp
        exp3_CPU.cpp
        exp3_Metal.cpp
        exp4.cpp
)

# Metal-cpp
target_include_directories(JK160510 PUBLIC metal-cpp)
target_include_directories(JK160510 PUBLIC metal-cpp/Foundation)
target_include_directories(JK160510 PUBLIC metal-cpp/Metal)
target_include_directories(JK160510 PUBLIC metal-cpp/MetalFX)
target_include_directories(JK160510 PUBLIC metal-cpp/QuartzCore)
target_include_directories(JK160510 PUBLIC metal-cpp/SingleHeader)

target_link_libraries(JK160510 PRIVATE "-framework Foundation")
target_link_libraries(JK160510 PRIVATE "-framework QuartzCore")
target_link_libraries(JK160510 PRIVATE "-framework Metal")

# Link MPI libraries - PRIVATE 表示链接的库仅对目标本身可见，不对其他依赖的目标传播
if (MPI_FOUND)
    target_link_libraries(JK160510 PRIVATE MPI::MPI_CXX)
else ()
    message(FATAL_ERROR "MPI not found!")
endif ()
# Link OpenCL libraries
if (OpenCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIRS})
    target_link_libraries(JK160510 PRIVATE ${OpenCL_LIBRARIES})
else ()
    # macOS 上直接使用系统框架
    message(STATUS "Using macOS system OpenCL framework.")
    target_link_libraries(JK160510 PRIVATE "-framework OpenCL")
endif ()
# Link Accelerate Framework libraries
target_link_libraries(JK160510 PRIVATE "-framework Accelerate")
