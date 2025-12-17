

cmake_minimum_required(VERSION 4.2.1)

project(fastOrderbook LANGUAGES CXX)

option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
