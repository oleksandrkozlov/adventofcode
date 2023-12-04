#include "input.h"

#include <range/v3/all.hpp>

#include <cassert>
#include <iostream>
#include <string>

auto main() -> int
{
    auto result1 = 0;
    auto result2 = 0;
    const auto red_cubes = 12;
    const auto green_cubes = 13;
    const auto blue_cubes = 14;
    auto id = 0;
    for (const auto line : Input | ranges::views::split('\n')) {
        auto is_possible = true;
        auto red_max = 0;
        auto green_max = 0;
        auto blue_max = 0;
        ++id;
        const auto games = line | ranges::views::split(':');
        for (const auto game : *ranges::next(ranges::cbegin(games)) | ranges::views::split(';')) {
            for (const auto game_set : game | ranges::views::split(',')) {
                const auto count_and_color = game_set | ranges::views::split(' ');
                const auto count_it = ranges::next(ranges::cbegin(count_and_color));
                const auto count = std::stoi(*count_it | ranges::to<std::string>());
                const auto color = *ranges::next(count_it) | ranges::to<std::string>();
                if ((color == "blue" && (count > blue_cubes)) ||
                    (color == "red" && (count > red_cubes)) ||
                    (color == "green" && (count > green_cubes))) {
                    is_possible = false;
                }
                if (color == "blue") {
                    blue_max = std::max(blue_max, count);
                }
                if (color == "red") {
                    red_max = std::max(red_max, count);
                }
                if (color == "green") {
                    green_max = std::max(green_max, count);
                }
            }
        }
        if (is_possible) {
            result1 += id;
        }
        result2 += (blue_max * red_max * green_max);
    }
    std::cout << result1 << " " << result2 << std::endl;
    assert(result1 == 2541);
    assert(result2 == 66016);
}
