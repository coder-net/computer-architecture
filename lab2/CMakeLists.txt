cmake_minimum_required(VERSION 3.15)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

add_executable(lab2 main.cpp modules/output.h modules/parallel_traversal.h modules/queues/dynamic_queue.h modules/queues_test.h modules/queues/fixed_mutex_queue.h modules/queues/fixed_atomic_queue.h)