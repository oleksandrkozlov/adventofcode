#!/usr/bin/env bash

set -e

# Validate input
# ------------------------------------------------------------
if [ $# -ne 2 ]; then
    echo "[ERROR] Usage: $0 <year> <day>"
    exit 1
fi

YEAR="$1"
DAY="$2"

if ! [[ "$YEAR" =~ ^[0-9]{4}$ ]]; then
    echo "[ERROR] Year must be a 4-digit number between 2015 and 2115."
    exit 1
fi
YEAR_NUM=$((10#$YEAR))
if (( YEAR_NUM < 2015 || YEAR_NUM > 2115 )); then
    echo "[ERROR] Year must be between 2015 and 2115."
    exit 1
fi

if ! [[ "$DAY" =~ ^[0-9]{1,2}$ ]]; then
    echo "[ERROR] Day must be between 01 and 25."
    exit 1
fi
DAY_NUM=$((10#$DAY))
if (( DAY_NUM < 1 || DAY_NUM > 25 )); then
    echo "[ERROR] Day must be between 01 and 25."
    exit 1
fi
DAY_PADDED=$(printf "%02d" "$DAY_NUM")

if [ -z "$AOC_SESSION" ]; then
    echo "[ERROR] AOC_SESSION is not set."
    exit 1
fi

if ! command -v wget >/dev/null 2>&1; then
    echo "[ERROR] wget is required but not installed or not on PATH."
    exit 1
fi

# Paths
# ------------------------------------------------------------
ROOT_CMAKE="CMakeLists.txt"
YEAR_DIR="src/${YEAR}"
DAY_DIR="${YEAR_DIR}/${DAY_PADDED}"
INPUT_PATH="${DAY_DIR}/input"
MAIN_CPP="${DAY_DIR}/main.cpp"
YEAR_CMAKE="${YEAR_DIR}/CMakeLists.txt"

EXEC_NAME="${YEAR}_${DAY_PADDED}"
EXEC_LINE="add_executable(${EXEC_NAME} ${DAY_PADDED}/main.cpp)"
ROOT_SUBDIR_LINE="add_subdirectory(src/${YEAR})"

# Ensure directory structure exists
# ------------------------------------------------------------
mkdir -p "$DAY_DIR"
echo "[INFO] Using directory: ${DAY_DIR}"

# Ensure root CMakeLists.txt contains add_subdirectory(src/<year>)
# ------------------------------------------------------------
if grep -Fq "$ROOT_SUBDIR_LINE" "$ROOT_CMAKE"; then
    echo "[SKIP] Root already includes src/${YEAR}"
else
    echo "[INFO] Adding year subdirectory to root CMakeLists..."
    echo "$ROOT_SUBDIR_LINE" >> "$ROOT_CMAKE"
    echo "[OK] Added: $ROOT_SUBDIR_LINE"
fi

# Ensure src/<year>/CMakeLists.txt exists
# ------------------------------------------------------------
if [ -f "$YEAR_CMAKE" ]; then
    echo "[SKIP] Year CMakeLists exists: ${YEAR_CMAKE}"
else
    echo "[INFO] Creating CMakeLists for year ${YEAR}..."
    cat > "$YEAR_CMAKE" <<EOF
cmake_minimum_required(VERSION 3.16)
project(AoC${YEAR})

EOF
    echo "[OK] Created ${YEAR_CMAKE}"
fi

# Ensure main.cpp exists
# ------------------------------------------------------------
if [ -f "$MAIN_CPP" ]; then
    echo "[SKIP] main.cpp exists: ${MAIN_CPP}"
else
    echo "[INFO] Creating main.cpp..."
    cat > "$MAIN_CPP" <<EOF
#include <bits/stdc++.h>

namespace {

namespace rv = std::views;
namespace rng = std::ranges;

constexpr auto input = std::invoke([] {
    static constexpr char result[] = {
#if 1
#embed "input"
#else
#embed "example"
#endif
    };
    return std::string_view{result, sizeof(result)};
});

} // namespace

auto main() -> int
{
    std::println("input: {} bytes", std::size(input));
}
EOF
    echo "[OK] Created ${MAIN_CPP}"
fi

# Ensure add_executable(...) exists in year CMakeLists
# ------------------------------------------------------------
if grep -Fq "$EXEC_LINE" "$YEAR_CMAKE"; then
    echo "[SKIP] Executable already exists in ${YEAR_CMAKE}"
else
    echo "[INFO] Adding executable to ${YEAR_CMAKE}"
    echo "$EXEC_LINE" >> "$YEAR_CMAKE"
    echo "[OK] Added: $EXEC_LINE"
fi

# Download input
# ------------------------------------------------------------
if [ -f "$INPUT_PATH" ]; then
    echo "[SKIP] Input already exists: ${INPUT_PATH}"
else
    echo "[INFO] Downloading input for ${YEAR} Day ${DAY_PADDED}..."
    wget_log=$(mktemp)
    if wget \
        --header="Cookie: session=${AOC_SESSION}" \
        "https://adventofcode.com/${YEAR}/day/${DAY_NUM}/input" \
        -O "$INPUT_PATH" >"$wget_log" 2>&1; then
        rm -f "$wget_log"
        echo "[OK] Downloaded input → ${INPUT_PATH}"
        cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
        BUILD_COMMAND="cmake --build build --target ${YEAR}_${DAY_PADDED}"
        RUN_COMMAND="./build/bin/${YEAR}_${DAY_PADDED}"
        echo "${BUILD_COMMAND} && ${RUN_COMMAND}"
        ${BUILD_COMMAND}
        ${RUN_COMMAND}
        echo "[DONE] Setup complete for AoC ${YEAR} Day ${DAY_PADDED}: https://adventofcode.com/${YEAR}/day/${DAY_NUM}"
        echo "src/${YEAR}/${DAY_PADDED}/main.cpp"
    else
        echo "[ERROR] Failed to download input for ${YEAR} Day ${DAY_PADDED}."
        echo "[ERROR] wget output:"
        cat "$wget_log"
        rm -f "$wget_log"
        rm -f "$INPUT_PATH"
        exit 1
    fi
fi

