#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

struct Number {
    std::int64_t value;
    std::int64_t index;
};

using Numbers = std::vector<Number>;

[[nodiscard]] auto parse_input(const std::string_view input) -> Numbers
{
    auto result = Numbers{};
    auto i = std::int64_t{};
    for (const auto range : input | ranges::views::split('\n')) {
        result.emplace_back(std::stoi(range | ranges::to<std::string>()), i++);
    }
    return result;
}

void shift(Numbers& numbers, const std::int64_t i)
{
    auto it = ranges::find_if(numbers, [&](const auto& n) { return n.index == i; });
    const auto tmp = it;
    const auto num = *it;
    const auto pos = std::distance(std::begin(numbers), it);
    const auto value = num.value;
    const auto size = static_cast<std::int64_t>(std::size(numbers) - 1);
    if (value == 0 || std::abs(value) % size == 0) {
        return;
    }
    auto distance = value;
    assert(distance != size);
    if (std::abs(distance) > size) {
        distance %= size;
    }
    const auto shift = pos + distance;
    if (shift > size) {
        distance -= size;
    } else if (shift <= 0) {
        distance += size;
    }
    std::advance(it, distance);
    numbers.erase(tmp);
    numbers.insert(it, 1, num);
}

[[nodiscard]] auto get_sum_of_coordinates(Numbers numbers, const std::size_t part) -> std::int64_t
{
    if (part == 2) {
        for (auto& n : numbers) {
            const auto decryption_key = 811589153;
            n.value *= decryption_key;
        }
    }
    const auto size = static_cast<std::int64_t>(numbers.size());
    const auto times = (part == 2) ? 10 : 1;
    for (auto t = 0; t < times; ++t) {
        for (auto i = std::int64_t{}; i < size; ++i) {
            shift(numbers, i);
        }
    }
    const auto it = ranges::find_if(numbers, [](const auto& e) { return e.value == 0; });
    assert(it != std::cend(numbers));
    const auto distance = std::distance(std::begin(numbers), it);
    const auto get_value = [&](const auto nth) {
        return numbers.at((nth + distance) % (size)).value;
    };
    return get_value(1000) + get_value(2000) + get_value(3000);
}

auto main() -> int
{
    const auto numbers = parse_input(Input);
    const auto result1 = get_sum_of_coordinates(numbers, 1);
    const auto result2 = get_sum_of_coordinates(numbers, 2);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 2827);
    assert(result2 == 7834270093909);
}
