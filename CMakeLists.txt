cmake_minimum_required (VERSION 2.8)
enable_testing()
project (instrument)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(CMAKE_CXX_FLAGS "-std=c++11")

add_subdirectory(cow_instrument)

