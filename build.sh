#!/bin/zsh

rm -rf build

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=clang++\
  -DUSE_BENCHMARK=ON
cmake --build build
