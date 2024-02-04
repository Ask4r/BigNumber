cmake_minimum_required(VERSION 3.27)
project(big_number_arithmetic)

set(CMAKE_CXX_STANDARD 20)

add_executable(big_number_arithmetic main.cpp
        big-number/BigNumber.cpp
        big-number/BigNumber.h
        big-number/vector-utils/VectorUtils.cpp
        big-number/vector-utils/VectorUtils.h
)
