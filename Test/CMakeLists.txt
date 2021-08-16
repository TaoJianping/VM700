cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")


project(VM700Test)

# install google test
message(STATUS "Start Install GoogleTest")
find_package(GTest CONFIG REQUIRED)
if (GTest_FOUND)
    message(Fount GTest !)
endif ()

# install google log
message(STATUS "Start Install glog")
find_package(glog CONFIG REQUIRED)
if (glog_FOUND)
    message(Fount glog !)
endif ()

find_package(absl CONFIG REQUIRED)

set(
        Headers
)

set(
        Sources
        testScanner.cpp
        testBasic.cpp
)

AUX_SOURCE_DIRECTORY(../Src ExecutableFile)
list(REMOVE_ITEM ExecutableFile "../Src/main.cpp")

ADD_EXECUTABLE(VM700Test ${Headers} ${Sources} ${ExecutableFile})
target_link_libraries(VM700Test GTest::gtest GTest::gtest_main glog::glog absl::strings absl::str_format)
target_include_directories(VM700Test PUBLIC ../Src)
