cmake_minimum_required(VERSION 3.16)
project(AoC2025)

add_executable(2025_01 01/main.cpp)
target_compile_options(2025_01 PRIVATE -fconstexpr-ops-limit=67108864)
add_executable(2025_02 02/main.cpp)
add_executable(2025_03 03/main.cpp)
add_executable(2025_04 04/main.cpp)
