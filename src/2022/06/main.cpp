#include "input.h"

#include <cassert>
#include <iostream>
#include <set>
#include <string>

[[nodiscard]] auto contains_duplicated_char(const std::string& s) -> bool
{
    auto check_unique = std::set<char>{};
    for (auto i = std::size_t{}; i < std::size(s); ++i) {
        if (!check_unique.insert(s.at(i)).second) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] auto get_number_of_chars(const std::string& input, const std::size_t marker) -> int
{
    for (auto i = std::size_t{}; i < std::size(input) - marker; ++i) {
        const auto four = input.substr(i, marker);
        auto has_duplicate = contains_duplicated_char(four);
        if (!has_duplicate) {
            return static_cast<int>(i + marker);
        }
    }
    return -1;
}

auto main() -> int
{
    const auto result1 = get_number_of_chars(std::string{Input}, 4);
    const auto result2 = get_number_of_chars(std::string{Input}, 14);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 1582);
    assert(result2 == 3588);
}
