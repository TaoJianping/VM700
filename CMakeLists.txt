cmake_minimum_required(VERSION 3.16)

# install google log

project(VM700)

find_package(glog CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(VM700 Src/main.cpp Src/common.h Src/chunk.h Src/Debug.cpp Src/Debug.h Src/chunk.cpp Src/value.h Src/value.cpp Src/vm.cpp Src/vm.h Src/VmStack.cpp Src/VmStack.h Src/Scanner.cpp Src/Scanner.h Src/Compiler.cpp Src/Compiler.h Src/Parser.cpp Src/Parser.h)
target_link_libraries(VM700 glog::glog absl::strings absl::str_format)