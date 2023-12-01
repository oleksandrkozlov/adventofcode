cmake_minimum_required(VERSION 3.24)

project(adventofcode LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(WITH_SYMENGINE "With SymEngine" ON)
option(WITH_GMPXX "With gmpxx" ON)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    option(WITH_CMAKE_FORMAT "With cmake-format" ON)
    option(WITH_CLANG_FORMAT "With clang-format" ON)
    option(WITH_CCACHE "With ccache" ON)
endif()

include(flags)
include(packages)

if (WITH_CMAKE_FORMAT)
    include(cmake-format)
endif()
if (WITH_CLANG_FORMAT)
    include(clang-format)
endif()
if (WITH_CCACHE)
    include(ccache)
endif()

add_subdirectory(src/2022)
add_subdirectory(src/2023)
