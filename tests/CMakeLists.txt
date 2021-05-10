cmake_minimum_required(VERSION 3.19)
project(testing)

find_package(Catch2 REQUIRED)

add_executable(tests tests.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain libsmcapi)

include(CTest)
include(Catch)
catch_discover_tests(tests)