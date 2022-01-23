cmake_minimum_required(VERSION 3.13)

option(ROKA_BUILD_TESTS "Whether to build Roka's test" OFF)

project(roka CXX)

add_library(roka INTERFACE)
target_include_directories(roka INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
cmake_policy(SET CMP0076 NEW)
target_sources(roka INTERFACE
    include/roka/roka.hpp
)
target_compile_features(roka INTERFACE cxx_std_17)

if(ROKA_BUILD_TESTS)
    add_subdirectory(src_test)
endif()
