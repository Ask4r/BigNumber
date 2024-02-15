cmake_minimum_required(VERSION 3.27)
project(bignumber)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp)
add_executable(bignumber_run ${SOURCE_FILES})

include_directories(bignumberlib)
add_subdirectory(bignumberlib)
target_link_libraries(bignumber_run bignumberlib_lib)
