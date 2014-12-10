cmake_minimum_required(VERSION 2.8.4)
project(SumMatrix)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(OpenCL REQUIRED)

include_directories(${OPENCL_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/khronos")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(cl/summatrix.cl ${CMAKE_CURRENT_BINARY_DIR}/cl/summatrix.cl COPYONLY)

set(SOURCE_FILES main.cpp)
add_executable(SumMatrix ${SOURCE_FILES})
target_link_libraries(SumMatrix ${OPENCL_LIBRARIES})