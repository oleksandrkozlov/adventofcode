#include "input.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

} // namespace

[[nodiscard]] auto split_string_by_delimeter(const std::string& str, const char delimter)
    -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};
    for (std::string line; std::getline(ss, line, delimter);)
        result.push_back(line);
    return result;
}

[[nodiscard]] auto get_size_of_assignment_pairs_contain_full_range(
    const std::vector<std::string>& pairs) -> int
{
    auto result = 0;
    auto group = std::vector<std::string>{};
    auto badges = std::vector<std::string>{};
    for (auto one_pair : pairs) {
        const auto two_sections = split_string_by_delimeter(one_pair, ',');
        auto first_section = two_sections.at(0);
        auto second_section = two_sections.at(1);
        auto first_range = split_string_by_delimeter(first_section, '-');
        auto second_range = split_string_by_delimeter(second_section, '-');
        auto first_range_begin = std::stoi(first_range.at(0));
        auto first_range_end = std::stoi(first_range.at(1));
        auto second_range_begin = std::stoi(second_range.at(0));
        auto second_range_end = std::stoi(second_range.at(1));
        auto first_rooms = std::vector<int>{};
        auto second_rooms = std::vector<int>{};
        for (auto i = first_range_begin; i <= first_range_end; ++i) {
            first_rooms.push_back(i);
        }
        for (auto i = second_range_begin; i <= second_range_end; ++i) {
            second_rooms.push_back(i);
        }
        std::sort(first_rooms.begin(), first_rooms.end());
        std::sort(second_rooms.begin(), second_rooms.end());
        auto same_rooms = std::vector<int>{};
        std::set_intersection(
            first_rooms.begin(),
            first_rooms.end(),
            second_rooms.begin(),
            second_rooms.end(),
            std::back_inserter(same_rooms));
        if (first_rooms == same_rooms || second_rooms == same_rooms) {
            ++result;
        }
    }
    return result;
}

[[nodiscard]] auto get_assignment_pairs_overlap_range(const std::vector<std::string>& pairs) -> int
{
    auto result = 0;
    for (const auto& one_pair : pairs) {
        const auto two_sections = split_string_by_delimeter(one_pair, ',');
        const auto get_sorted_rooms = [](const auto& sections) {
            auto rooms = std::vector<int>{};
            const auto range = split_string_by_delimeter(sections, '-');
            for (auto room = std::stoi(range.at(0)); room <= std::stoi(range.at(1)); ++room) {
                rooms.push_back(room);
            }
            std::sort(rooms.begin(), rooms.end());
            return rooms;
        };
        auto first_rooms = get_sorted_rooms(two_sections.at(0));
        auto second_rooms = get_sorted_rooms(two_sections.at(1));
        auto same_rooms = std::vector<int>{};
        std::set_intersection(
            first_rooms.begin(),
            first_rooms.end(),
            second_rooms.begin(),
            second_rooms.end(),
            std::back_inserter(same_rooms));
        if (!same_rooms.empty()) {
            ++result;
        }
    }
    return result;
}

[[nodiscard]] auto get_assignment_pairs(
    const std::vector<std::string>& pairs,
    const std::size_t part) -> int
{
    if (part == 1) {
        return get_size_of_assignment_pairs_contain_full_range(pairs);
    }
    assert(part == 2);
    return get_assignment_pairs_overlap_range(pairs);
}

int main()
{
    const auto pairs = split_string_by_delimeter(std::string{Input}, '\n');
    const auto result1 = get_assignment_pairs(pairs, 1);
    const auto result2 = get_assignment_pairs(pairs, 2);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 483);
    assert(result2 == 874);
}
