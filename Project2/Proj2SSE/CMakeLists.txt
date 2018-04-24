cmake_minimum_required(VERSION 3.10)
project(Proj2SSE)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-msse4.2")

add_executable(Proj2SSE main.cpp)

