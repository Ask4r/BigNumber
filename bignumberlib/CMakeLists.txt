project(bignumberlib)

set(HEADER_FILES big_number.h)
set(SOURCE_FILES big_number.cpp)

add_library(bignumberlib_lib ${HEADER_FILES} ${SOURCE_FILES})

include_directories(vectorutilslib)
add_subdirectory(vectorutilslib)
target_link_libraries(bignumberlib_lib vectorutilslib_lib)