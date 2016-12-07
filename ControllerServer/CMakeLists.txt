# cmake_minimum_required(VERSION <specify CMake version here>)
project(ControllerServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ControllerTaskProcess.h ControllerTaskProcess.cpp)
add_executable(ControllerServer ${SOURCE_FILES})