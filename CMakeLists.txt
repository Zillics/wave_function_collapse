cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


project(wfc)
set(EXECUTABLE_NAME "wfc")
add_executable(${EXECUTABLE_NAME} main.cpp wfc.cpp )

