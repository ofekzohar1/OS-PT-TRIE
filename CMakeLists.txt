cmake_minimum_required(VERSION 3.16)
project(os1 C)

set(CMAKE_C_STANDARD 11)

add_executable(os1 os.c os.h pt.c)
target_link_libraries(os1 PRIVATE m)
