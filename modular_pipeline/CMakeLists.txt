cmake_minimum_required(VERSION 3.1)

project(modular_pipeline)

set(CMAKE_CXX_STANDARD 11)

include(../cmake/SetEnv.cmake)
include(../cmake/Glog.cmake)

include_directories("include")

add_executable(modular_pipeline main.cpp include/pipeline_module.hpp)
target_link_libraries(modular_pipeline PRIVATE glog::glog)
