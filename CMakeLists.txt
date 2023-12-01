cmake_minimum_required(VERSION 3.24)

project(adventofcode LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(WITH_SYMENGINE "With SymEngine" ON)
option(WITH_GMPXX "With gmpxx" ON)

include(flags)
include(packages)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    include(cmake-format)
    include(clang-format)
    include(ccache)
endif()

add_subdirectory(src/2022)
