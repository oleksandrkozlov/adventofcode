#include "input.h"

#include <range/v3/all.hpp>

#include <cassert>
#include <format>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

[[nodiscard]] auto find_first(
    const std::string_view find_in,
    const std::vector<std::string_view>& find_what) -> std::string
{
    auto n = std::numeric_limits<std::size_t>::max();
    auto w = std::string{};
    for (const auto& what : find_what) {
        const auto found = find_in.find(what);
        if (std::string_view::npos != found) {
            if (found < n) {
                n = found;
                w = what;
            }
        }
    }
    return std::string{find_in.substr(n, std::size(w))};
}

[[nodiscard]] auto find_last(
    const std::string_view find_in,
    const std::vector<std::string_view>& find_what) -> std::string
{
    auto n = std::size_t{0};
    auto w = std::string{};
    for (const auto& what : find_what) {
        const auto found = find_in.rfind(what);
        if (std::string_view::npos != found) {
            if (found >= n) {
                n = found;
                w = what;
            }
        }
    }
    return std::string{find_in.substr(n, std::size(w))};
}

[[nodiscard]] auto to_int(const std::string& digit) -> int
{
    if (std::size(digit) == 1U) {
        return std::stoi(digit);
    }
    return std::map<std::string, int>{
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9}}
        .at(digit);
}

[[nodiscard]] auto get_calibrated_value(const int part) -> int
{
    auto digits = std::vector<std::string_view>{"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    if (part == 2) {
        const auto numbers = std::vector<std::string_view>{
            "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
        digits.insert(std::end(digits), std::cbegin(numbers), std::cend(numbers));
    }
    return ranges::accumulate(
        Input // clang-format off
        | ranges::views::split('\n')
        | ranges::views::transform([&](const auto range) {
            using namespace std::literals;
            const auto line = range | ranges::to<std::string>();
            const auto first = find_first(line, digits);
            const auto last = find_last(line,  digits);
            return std::stoi(std::format("{}{}", to_int(first), to_int(last)));
        }), 0); // clang-format on
}

auto main() -> int
{
    const auto result1 = get_calibrated_value(1);
    const auto result2 = get_calibrated_value(2);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 55208);
    assert(result2 == 54578);
}
