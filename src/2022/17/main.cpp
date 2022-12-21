#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>

struct Point {
    std::int64_t x = 0;
    std::int64_t y = 0;
    auto operator<=>(const Point& p) const = default;
};

using Grid = std::unordered_set<Point>;
using Rock = std::unordered_set<Point>;
using Pattern = std::unordered_set<Point>;
using Tick = std::int64_t;
using RockType = std::int64_t;
using FallenRocks = std::size_t;
using Height = std::int64_t;
using State = std::tuple<Tick, RockType, Pattern>;
using States = std::unordered_map<State, std::pair<FallenRocks, Height>>;

template<>
struct std::hash<Point> {
    [[nodiscard]] auto operator()(const Point& p) const -> std::size_t
    {
        auto result = std::size_t{};
        boost::hash_combine(result, std::hash<decltype(p.x)>{}(p.x));
        boost::hash_combine(result, std::hash<decltype(p.y)>{}(p.y));
        return result;
    }
};

template<>
struct std::hash<std::unordered_set<Point>> {
    [[nodiscard]] auto operator()(const std::unordered_set<Point>& points) const -> std::size_t
    {
        auto result = std::size_t{};
        for (const auto& point : points) {
            boost::hash_combine(result, std::hash<Point>{}(point));
        }
        return result;
    }
};

template<>
struct std::hash<State> {
    [[nodiscard]] auto operator()(const State& state) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto& [fallen_rocks, rock_type, pattern] = state;
        boost::hash_combine(result, std::hash<FallenRocks>{}(fallen_rocks));
        boost::hash_combine(result, std::hash<RockType>{}(rock_type));
        boost::hash_combine(result, std::hash<Pattern>{}(pattern));
        return result;
    }
};

[[nodiscard]] auto get_rock_height(const Rock& rock) -> std::int64_t
{
    auto result = std::numeric_limits<std::int64_t>::min();
    for (const Point& p : rock) {
        result = std::max(result, p.y);
    }
    return result + 1;
}

[[nodiscard]] auto shift_grid(const Grid& g, const std::int64_t diff_y) -> Grid
{
    auto result = std::unordered_set<Point>{};
    for (const auto& p : g) {
        result.emplace(p.x, p.y + diff_y);
    }
    return result;
}

[[nodiscard]] auto get_pattern(const Grid& grid) -> Pattern
{
    auto result = Pattern{};
    for (const auto& p : grid) {
        const auto arbitrary_chunk = 7;
        if (p.y <= arbitrary_chunk) {
            result.emplace(p.x, p.y);
        }
    }
    return result;
}

[[nodiscard]] auto get_diff_x(const std::string_view input, const std::int64_t tick) -> std::int64_t
{
    assert(tick >= 0 && tick < static_cast<std::int64_t>(std::size(input)));
    return input.at(tick) == '>' ? 1 : -1;
}

auto print_grid(const Grid& grid, const std::int64_t grid_width, const std::int64_t grid_height)
    -> void
{
    for (auto y = std::int64_t{}; y < grid_height; ++y) {
        fmt::print("{} ", y);
        for (auto x = std::int64_t{}; x < grid_width; ++x) {
            if (grid.contains({x, y})) {
                fmt::print("@");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
    fmt::print("\n");
};

[[nodiscard]] auto create_rocks(const std::int64_t x)
{
    return std::array{
        Rock{{0 + x, 0}, {1 + x, 0}, {2 + x, 0}, {3 + x, 0}},
        Rock{{1 + x, 0}, {0 + x, 1}, {1 + x, 1}, {2 + x, 1}, {1 + x, 2}},
        Rock{{2 + x, 0}, {2 + x, 1}, {0 + x, 2}, {1 + x, 2}, {2 + x, 2}},
        Rock{{0 + x, 0}, {0 + x, 1}, {0 + x, 2}, {0 + x, 3}},
        Rock{{0 + x, 0}, {0 + x, 1}, {1 + x, 0}, {1 + x, 1}}};
}

[[nodiscard]] auto get_rocks_heights(const auto& rocks)
{
    auto result = std::array<std::int64_t, 5>{};
    for (auto i = std::size_t{}; i < std::size(rocks); ++i) {
        result.at(i) = get_rock_height(rocks.at(i));
    }
    return result;
}

[[nodiscard]] auto create_grid_with_floor(const std::int64_t grid_width) -> Grid
{
    auto result = Grid{};
    for (auto x = std::int64_t{}; x < grid_width; ++x) {
        result.emplace(x, 0);
    }
    return result;
}

auto get_tower_height(const std::string_view input, const std::size_t rocks_to_fall) -> std::int64_t
{
    const auto shift_y = 3;
    const auto shift_x = 2;
    const auto grid_width = std::int64_t{7};
    const auto rocks = create_rocks(shift_x);
    const auto rocks_hights = get_rocks_heights(rocks);
    auto grid = create_grid_with_floor(grid_width);
    auto top = std::int64_t{};
    auto bottom = std::numeric_limits<std::int64_t>::min();
    auto height = Height{};
    auto additional_height = Height{};
    auto states = States{};
    auto fallen_rocks = FallenRocks{};
    auto tick = Tick{-1};
    auto rock_type = RockType{-1};
    while (fallen_rocks != rocks_to_fall) {
        ++fallen_rocks;
        ++rock_type;
        if (rock_type == static_cast<std::int64_t>(std::size(rocks))) {
            rock_type = 0;
        }
        const auto& rock = rocks.at(rock_type);
        const auto diff_y = shift_y - top + rocks_hights.at(rock_type);
        top += diff_y;
        bottom += diff_y;
        grid = shift_grid(grid, diff_y);
        auto xx = std::int64_t{};
        auto yy = std::int64_t{};
        while (true) {
            ++tick;
            if (tick == static_cast<std::int64_t>(std::size(input))) {
                tick = 0;
            }
            const auto diff_x = get_diff_x(input, tick);
            const auto shift = diff_x + xx;
            if (ranges::all_of(rock, [&](const auto& p) {
                    const auto x = p.x + shift;
                    const auto y = p.y + yy;
                    return (x < grid_width && x >= 0) && !grid.contains({x, y});
                })) {
                xx += diff_x;
            }
            if (ranges::any_of(rock, [&](const auto& p) {
                    return grid.contains({p.x + xx, p.y + yy + 1});
                })) {
                for (const auto& p : rock) {
                    const auto x = p.x + xx;
                    const auto y = p.y + yy;
                    grid.emplace(x, y);
                    top = std::min(top, y);
                    bottom = std::max(bottom, y);
                    height = bottom - top + 1;
                }
                const auto state = std::make_tuple(tick, rock_type, get_pattern(grid));
                if (states.contains(state)) {
                    const auto [old_fallen_rocks, old_height] = states.at(state);
                    const auto height_diff = height - old_height;
                    const auto fallen_rocks_diff = fallen_rocks - old_fallen_rocks;
                    const auto rocks_left = rocks_to_fall - fallen_rocks;
                    const auto multiply =
                        static_cast<std::int64_t>(std::floor(rocks_left / fallen_rocks_diff));
                    additional_height += multiply * height_diff;
                    fallen_rocks += multiply * fallen_rocks_diff;
                } else {
                    states.emplace(state, std::make_pair(fallen_rocks, height));
                }
                break;
            }
            ++yy;
        }
    }
    // print_grid(grid, grid_width, bottom + 1);
    return height + additional_height;
}

auto main() -> int
{
    const auto result1 = get_tower_height(Input, 2022);
    const auto result2 = get_tower_height(Input, 1000000000000);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 3119);
    assert(result2 == 1536994219669);
}
