cmake_minimum_required(VERSION 3.1)

project(concurrent_queue)

include(../cmake/SetEnv.cmake)

set(CMAKE_CXX_STANDARD 11)

add_executable(concurrent_queue main.cpp)
