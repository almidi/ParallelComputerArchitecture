cmake_minimum_required(VERSION 3.10)
project(Proj2MPI C)

set(CMAKE_C_STANDARD 11)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_C_COMPILER mpicc)

add_executable(Proj2MPI main.c)

