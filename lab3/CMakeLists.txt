cmake_minimum_required(VERSION 3.15)
project(lab3)

set(CMAKE_CXX_STANDARD 11 LINK_FLAGS "${OpenMP_CXX_FLAGS}")

add_executable(lab3 main.cpp vector_by_vector.h matrix_by_vector.h)