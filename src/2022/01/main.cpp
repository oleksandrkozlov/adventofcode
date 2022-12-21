#include "input.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

[[nodiscard]] auto split_string_by_newline(const std::string& str) -> int
{
    auto result = 0;
    auto ss = std::stringstream{str};
    for (std::string line; std::getline(ss, line, '\n');) {
        result += std::stoi(line);
    }
    return result;
}

[[nodiscard]] auto split_string_by_delimeter(std::string s, const std::string& delimiter)
    -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return result;
}

auto main() -> int
{
    auto cals = std::vector<int>{};
    auto vals = split_string_by_delimeter(std::string{Input}, "\n\n");
    for (const auto& v : vals) {
        cals.push_back(split_string_by_newline(v));
    }
    auto first_it = std::max_element(cals.begin(), cals.end());
    auto first = *first_it;
    cals.erase(first_it);
    auto second_it = std::max_element(cals.begin(), cals.end());
    auto second = *second_it;
    cals.erase(second_it);
    auto third_it = std::max_element(cals.begin(), cals.end());
    auto third = *third_it;
    const auto result = first + second + third;
    std::cout << first << std::endl << result << std::endl;
    assert(first == 73211);
    assert(result == 213958);
}
