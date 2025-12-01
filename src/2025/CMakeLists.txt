cmake_minimum_required(VERSION 3.16)
project(AoC2025)

add_executable(2025_01 01/main.cpp)
target_compile_options(2025_01 PRIVATE -fconstexpr-ops-limit=67108864)
