cmake_minimum_required(VERSION 3.19)
project(tests)

find_package(Catch2 REQUIRED)

add_executable(tests tests.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain libsmcapi
        boost_system boost_thread boost_serialization pthread)

file(COPY ../examples/Comm/CommConfig.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
include(CTest)
include(Catch)
catch_discover_tests(tests)