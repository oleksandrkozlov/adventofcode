#include "input.h"

#include <range/v3/all.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <sstream>
#include <vector>

namespace {

namespace rng = ranges;

constexpr auto SplitByNewLine = std::views::split('\n');

auto get_priority(const std::string_view str) -> std::size_t
{
    // clang-format off
    if (str.starts_with("a")) { return 1; }
    if (str.starts_with("b")) { return 2; }
    if (str.starts_with("c")) { return 3; }
    if (str.starts_with("d")) { return 4; }
    if (str.starts_with("e")) { return 5; }
    if (str.starts_with("f")) { return 6; }
    if (str.starts_with("g")) { return 7; }
    if (str.starts_with("h")) { return 8; }
    if (str.starts_with("i")) { return 9; }
    if (str.starts_with("j")) { return 10; }
    if (str.starts_with("k")) { return 11; }
    if (str.starts_with("l")) { return 12; }
    if (str.starts_with("m")) { return 13; }
    if (str.starts_with("n")) { return 14; }
    if (str.starts_with("o")) { return 15; }
    if (str.starts_with("p")) { return 16; }
    if (str.starts_with("q")) { return 17; }
    if (str.starts_with("r")) { return 18; }
    if (str.starts_with("s")) { return 19; }
    if (str.starts_with("t")) { return 20; }
    if (str.starts_with("u")) { return 21; }
    if (str.starts_with("v")) { return 22; }
    if (str.starts_with("w")) { return 23; }
    if (str.starts_with("x")) { return 24; }
    if (str.starts_with("y")) { return 25; }
    if (str.starts_with("z")) { return 26; }
    if (str.starts_with("A")) { return 27; }
    if (str.starts_with("B")) { return 28; }
    if (str.starts_with("C")) { return 29; }
    if (str.starts_with("D")) { return 30; }
    if (str.starts_with("E")) { return 31; }
    if (str.starts_with("F")) { return 32; }
    if (str.starts_with("G")) { return 33; }
    if (str.starts_with("H")) { return 34; }
    if (str.starts_with("I")) { return 35; }
    if (str.starts_with("J")) { return 36; }
    if (str.starts_with("K")) { return 37; }
    if (str.starts_with("L")) { return 38; }
    if (str.starts_with("M")) { return 39; }
    if (str.starts_with("N")) { return 40; }
    if (str.starts_with("O")) { return 41; }
    if (str.starts_with("P")) { return 42; }
    if (str.starts_with("Q")) { return 43; }
    if (str.starts_with("R")) { return 44; }
    if (str.starts_with("S")) { return 45; }
    if (str.starts_with("T")) { return 46; }
    if (str.starts_with("U")) { return 47; }
    if (str.starts_with("V")) { return 48; }
    if (str.starts_with("W")) { return 49; }
    if (str.starts_with("X")) { return 50; }
    if (str.starts_with("Y")) { return 51; }
    if (str.starts_with("Z")) { return 52; }
    // clang-format on
    return 0;
}

[[nodiscard]] auto split_string_by_newline(const std::string& str) -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};
    for (std::string line; std::getline(ss, line, '\n');)
        result.push_back(line);
    return result;
}

constexpr auto sum(auto&& c, auto action) noexcept
{
    return std::accumulate(c.begin(), c.end(), 0U, action);
}

[[nodiscard]] auto get_sum_of_priorities() -> std::size_t
{
    std::vector<std::string> v;
    for (auto rucksack : Input | SplitByNewLine) {
        auto two_compartments = rucksack | rng::views::chunk(rucksack.size() / 2);
        auto first = two_compartments.at(0);
        auto second = two_compartments.at(1);
        auto first_str = std::string{first.begin(), first.end()};
        auto second_str = std::string{second.begin(), second.end()};

        std::string intersection;
        std::ranges::sort(first_str);
        std::ranges::sort(second_str);
        std::ranges::set_intersection(first_str, second_str, std::back_inserter(intersection));
        v.push_back(intersection);
    }
    return sum(v, [](auto sum_priority, auto str) { return sum_priority + get_priority(str); });
}

[[nodiscard]] auto get_sum_of_priorities_with_badge() -> std::size_t
{
    const auto items = split_string_by_newline(std::string{Input});
    auto group = std::vector<std::string>{};
    auto badges = std::vector<std::string>{};
    auto i = 0;
    for (auto item : items) {
        i++;
        group.push_back(item);
        if (i == 3) {
            auto first = group.at(0);
            std::sort(first.begin(), first.end());
            auto second = group.at(1);
            std::sort(second.begin(), second.end());
            auto first_second = std::string{};
            std::set_intersection(
                first.begin(),
                first.end(),
                second.begin(),
                second.end(),
                std::back_inserter(first_second));
            auto third = group.at(2);
            std::sort(third.begin(), third.end());
            auto second_third = std::string{};
            std::set_intersection(
                second.begin(),
                second.end(),
                third.begin(),
                third.end(),
                std::back_inserter(second_third));
            auto three = std::string{};
            std::set_intersection(
                first_second.begin(),
                first_second.end(),
                second_third.begin(),
                second_third.end(),
                std::back_inserter(three));
            badges.push_back(three);
            group.clear();
            i = 0;
        }
    }
    auto result = std::size_t{};
    for (const auto& badge : badges) {
        result += get_priority(badge);
    }
    return result;
}

[[nodiscard]] auto get_sum_of_priorities(const std::size_t part) -> std::size_t
{
    if (part == 1) {
        return get_sum_of_priorities();
    }
    assert(part == 2);
    {
        return get_sum_of_priorities_with_badge();
    }
}

} // namespace

int main()
{
    const auto result1 = get_sum_of_priorities(1);
    const auto result2 = get_sum_of_priorities(2);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 7872);
    assert(result2 == 2497);
}
