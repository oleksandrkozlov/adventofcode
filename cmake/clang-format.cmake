find_program(CLANG_FORMAT_PROGRAM clang-format)

if(NOT CLANG_FORMAT_PROGRAM)
    message(FATAL_ERROR "No program 'clang-format' found")
endif()

set(FIND_SOURCES
    "find ${PROJECT_SOURCE_DIR} -type f \\( -iname \\*.cpp -o -iname \\*.hpp \\) -not -path '${CMAKE_BINARY_DIR}/*'"
)

add_custom_target(
    format-cpp VERBATIM COMMAND bash -c "${FIND_SOURCES} | xargs -n 1 ${CLANG_FORMAT_PROGRAM} -i"
)
