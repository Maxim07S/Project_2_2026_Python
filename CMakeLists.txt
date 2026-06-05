cmake_minimum_required(VERSION 3.14)
project(crypto_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(crypto main.cpp)

enable_testing()
add_executable(unit_tests tests.cpp)

target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR})

add_test(NAME unit_tests COMMAND unit_tests)