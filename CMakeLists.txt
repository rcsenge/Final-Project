cmake_minimum_required(VERSION 3.29)
project(nagyHazi C)

set(CMAKE_C_STANDARD 11)

add_executable(nagyHazi main.c debugmalloc.h)
