cmake_minimum_required(VERSION 3.21)

project(wideint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WIDEINT_BUILD_TESTS "Build tests" OFF)
option(WIDEINT_BUILD_CUDA_TESTS "Build CUDA-based tests" OFF)

add_library(wideint INTERFACE)
target_include_directories(wideint INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(wideint::wideint ALIAS wideint)

if(WIDEINT_BUILD_TESTS)
    enable_testing()

    add_executable(test_wideint ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_wideint.cpp)
    target_compile_features(test_wideint PRIVATE cxx_std_17)
    target_link_libraries(test_wideint PRIVATE wideint::wideint)
    add_test(NAME test_wideint COMMAND test_wideint)

    if(WIDEINT_BUILD_CUDA_TESTS)
        enable_language(CUDA)

        add_executable(test_wideint_cuda ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_wideint_cuda.cu)
        set_target_properties(test_wideint_cuda PROPERTIES CUDA_ARCHITECTURES native)
        target_compile_features(test_wideint_cuda PRIVATE cuda_std_17)
        target_link_libraries(test_wideint_cuda PRIVATE wideint::wideint)
        add_test(NAME test_wideint_cuda COMMAND test_wideint_cuda)
    endif()
endif()
