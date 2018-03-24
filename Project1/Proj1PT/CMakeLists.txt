cmake_minimum_required(VERSION 3.9)
project(Proj1PT C)

set(CMAKE_C_STANDARD)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(Proj1PT main.c)