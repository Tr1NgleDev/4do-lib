cmake_minimum_required(VERSION 3.28)
project(test)

set(CMAKE_CXX_STANDARD 20)

add_executable(test main.cpp)
target_include_directories(test PRIVATE "../headers/")
