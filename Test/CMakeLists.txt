cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")


project(cloxTests)

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


include_directories(../Src)



set(
        Headers
)

set(
        Sources
        testScanner.cpp
)


set(
        ExecutableFile
        ../Src/Scanner.cpp
        ../Src/Scanner.h
)


ADD_EXECUTABLE(LoxTest ${Headers} ${Sources} ${ExecutableFile})
target_link_libraries(LoxTest GTest::gtest GTest::gtest_main glog::glog absl::strings absl::str_format)