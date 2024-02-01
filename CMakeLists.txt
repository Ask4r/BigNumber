cmake_minimum_required(VERSION 3.27)
project(big_number_arithmetic)

set(CMAKE_CXX_STANDARD 20)

add_executable(big_number_arithmetic main.cpp
        big-number/BigNumber.cpp
        big-number/BigNumber.h
        big-number/addition-and-subtraction/BigNumberAdditionAndSubtraction.cpp
        big-number/addition-and-subtraction/BigNumberAdditionAndSubtraction.h
        big-number/constructors/BigNumberConstructors.cpp
        big-number/constructors/BigNumberConstructors.h
        big-number/representation/BigNumberRepresentation.cpp
        big-number/representation/BigNumberRepresentation.h
        big-number/utils/BigNumberUtils.cpp
        big-number/utils/BigNumberUtils.h
        big-number/getters/BigNumberGetters.h
        big-number/getters/BigNumberGetters.cpp
        big-number/polymorphisms/BigNumberPolymorphisms.cpp
        big-number/polymorphisms/BigNumberPolymorphisms.h
        big-number/multiplication-and-division/BigNumberMultiplicationAndDivision.cpp
        big-number/multiplication-and-division/BigNumberMultiplicationAndDivision.h
        big-number/comparison/BigNumberComparison.h
        big-number/comparison/BigNumberComparison.cpp
)
