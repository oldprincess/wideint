cmake_minimum_required(VERSION 3.20)

project(wideint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WIDEINT_BUILD_CUDA_TESTS "Build CUDA-based tests" OFF)

add_library(wideint INTERFACE)
target_include_directories(wideint INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_wideint tests/test_wideint.cpp)
target_link_libraries(test_wideint PRIVATE wideint)

enable_testing()
add_test(NAME test_wideint COMMAND test_wideint)

if(WIDEINT_BUILD_CUDA_TESTS)
    enable_language(CUDA)
    add_executable(test_wideint_cuda tests/test_wideint_cuda.cu)
    target_link_libraries(test_wideint_cuda PRIVATE wideint)
    target_compile_features(test_wideint_cuda PRIVATE cuda_std_17)
    add_test(NAME test_wideint_cuda COMMAND test_wideint_cuda)
endif()
