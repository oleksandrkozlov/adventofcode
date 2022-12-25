#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <array>
#include <cassert>
#include <climits>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_set>

struct Point {
    auto operator<=>(const Point& point) const = default;
    int x{};
    int y{};
};

template<>
struct std::hash<Point> {
    [[nodiscard]] auto operator()(const Point& p) const -> std::size_t
    {
        auto result = std::size_t{};
        boost::hash_combine(result, std::hash<int>{}(p.x));
        boost::hash_combine(result, std::hash<int>{}(p.y));
        return result;
    }
};

template<>
struct fmt::formatter<Point> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const Point& p, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

using Map = std::unordered_set<Point>;
using Moves = std::vector<std::pair<Point, Point>>;

[[nodiscard]] auto get_map(const std::string_view input) -> Map
{
    auto result = Map{};
    auto p = Point{};
    for (const auto row : input | ranges::views::split('\n')) {
        p.x = {};
        for (const auto c : row) {
            if (c == '#') {
                result.emplace(p);
            }
            ++p.x;
        }
        ++p.y;
    };
    return result;
}

[[nodiscard]] auto are_all_points_around_free(const Map& map, const Point& point) -> bool
{
    constexpr auto dir = std::array{
        Point{-1, -1},
        Point{-1, 0},
        Point{-1, 1},
        Point{0, -1},
        Point{0, 1},
        Point{1, -1},
        Point{1, 0},
        Point{1, 1}};
    return ranges::none_of(dir, [&](const auto& p) {
        return map.contains({point.x + p.x, point.y + p.y});
    });
}

[[nodiscard]] auto get_proposed_move(const Map& map, const Point& point, const auto& dirs)
    -> std::optional<Point>
{
    auto result = std::optional<Point>{};
    for (const auto& dir : dirs) {
        if (ranges::none_of(dir, [&](const auto& p) {
                return map.contains({point.x + p.x, point.y + p.y});
            })) {
            const auto& p = dir.at(1);
            return Point{point.x + p.x, point.y + p.y};
        }
    }
    return result;
}

[[nodiscard]] auto get_min_max(const Map& map) -> std::pair<Point, Point>
{
    auto result = std::pair{Point{INT_MAX, INT_MAX}, Point{INT_MIN, INT_MIN}};
    auto& [min, max] = result;
    for (const auto& p : map) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
    }
    return result;
}

[[nodiscard]] auto simulate_round(Map map, const auto& dirs) -> Map
{
    auto moves = Moves{};
    for (const auto& point : map) {
        if (are_all_points_around_free(map, point)) {
            continue;
        }
        if (const auto proposed_move = get_proposed_move(map, point, dirs);
            proposed_move.has_value()) {
            moves.emplace_back(point, proposed_move.value());
        }
    }
    if (std::empty(moves)) {
        return map;
    }
    // clang-format off
    moves = ranges::actions::sort(moves, [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; })
        | ranges::views::chunk_by([](const auto& lhs, const auto& rhs) { return lhs.second == rhs.second; })
        | ranges::views::filter([](const auto& other) { return ranges::size(other) == 1U; })
        | ranges::views::join 
        | ranges::to<Moves>;
    // clang-format on 
    for (const auto& [from, to] : moves) {
        map.erase(from);
        assert(!map.contains(to));
        map.emplace(to);
    }
    return map;
}

[[nodiscard]] auto get_empty_points(const Map& map) -> int
{
    auto result = int{};
    const auto [min, max] = get_min_max(map);
    for (auto y = min.y; y <= max.y; ++y) {
        for (auto x = min.x; x <= max.x; ++x) {
            if (!map.contains({x, y})) {
                ++result;
            }
        }
    }
    return result;
}

[[maybe_unused]] auto print_map(const Map& map) -> void
{
    const auto [min, max] = get_min_max(map);
    for (auto y = min.y; y <= max.y; ++y) {
        fmt::print("{} ", y);
        for (auto x = min.x; x <= max.x; ++x) {
            if (const auto p = Point{x, y}; map.contains(p)) {
                fmt::print("#");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

[[nodiscard]] auto simulate_rounds(Map map) -> std::pair<int, int> {
    auto dirs = std::deque{
        std::array{Point{-1, -1}, Point{0, -1}, Point{1, -1}},
        std::array{Point{-1, 1}, Point{0, 1}, Point{1, 1}},
        std::array{Point{-1, -1}, Point{-1, 0}, Point{-1, 1}},
        std::array{Point{1, -1}, Point{1, 0}, Point{1, 1}},

    };
    auto result1 = 0;
    auto result2 = 0;
    auto round = 0;
    while (true) {
        ++round;
        const auto map_after_round = simulate_round(map, dirs);
        if (round == 10) {
            result1 = get_empty_points(map_after_round);
        }
        if (map == map_after_round) {
            result2 = round;
            break;
        }
        map = map_after_round;
        dirs.push_back(dirs.front());
        dirs.pop_front();
    }
    return {result1, result2};
}

auto main() -> int
{
    const auto [result1, result2] = simulate_rounds(get_map(Input));
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 3800);
    assert(result2 == 916);
}
