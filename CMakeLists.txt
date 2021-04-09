cmake_minimum_required(VERSION 3.17)

# install google log

project(VM700)

find_package(glog CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(VM700 Src/main.cpp Src/common.h Src/chunk.h)
target_link_libraries(VM700 PRIVATE glog::glog)