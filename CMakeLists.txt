
cmake_minimum_required(VERSION 4.20)

project(fastOrderbook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wpedantic")

add_subdirectory(src)
