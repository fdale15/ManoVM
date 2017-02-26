cmake_minimum_required(VERSION 3.6)
project(ManoVM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES ../../src/main.cpp ../../src/ManoMV.h)
add_executable(ManoVM ${SOURCE_FILES})