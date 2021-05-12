cmake_minimum_required(VERSION 3.19)
project(libsmcapi)

set(CMAKE_CXX_STANDARD 14)
include_directories(include)

add_library(libsmcapi SHARED library.cpp library.h examples/Comm/comm_example.cpp include/comm/Comm.hpp include/infra/ConfigFile.h src/infra/ConfigFIle.cpp examples/example.h)

# set environment variable to ON if you want to enable testing
# if (BUILD_TESTING)
enable_testing()
add_subdirectory(tests)
# endif()