project(Ex2)
cmake_minimum_required(VERSION 3.0.0)
set(C_STANDARD C99)
set(CXX_STANDARD C++17)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_library(color SHARED Color.cpp)
