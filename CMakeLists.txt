cmake_minimum_required(VERSION 3.19)
project(libsmcapi)

set(CMAKE_CXX_STANDARD 14)

add_library(libsmcapi SHARED library.cpp library.h)

# set environment variable to ON if you want to enable testing
# if (BUILD_TESTING)
enable_testing()
add_subdirectory(tests)
# endif()