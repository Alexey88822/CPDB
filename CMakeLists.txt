# cmake_minimum_required(VERSION <specify CMake version here>)
project(CPDB)

set(CMAKE_CXX_STANDARD 14)

add_executable(CPDB main.cpp)
target_link_libraries(CPDB libodbc32.a)