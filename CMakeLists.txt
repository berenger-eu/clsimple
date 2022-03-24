cmake_minimum_required (VERSION 3.0)
project (CLSIMPLE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test.exe tests/test.cpp)
target_include_directories(test.exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

