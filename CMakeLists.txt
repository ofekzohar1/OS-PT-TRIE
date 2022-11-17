cmake_minimum_required(VERSION 3.23)
project(os1 C)

set(CMAKE_C_STANDARD 11)

add_executable(os1 os.c os.h pt.c)
