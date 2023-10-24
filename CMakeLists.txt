cmake_minimum_required(VERSION 3.20)
project(Pool_allocator)

set(CMAKE_CXX_STANDARD 17)

add_executable(Pool_allocator main.cpp lib/Allocator.cpp lib/Allocator.h)
