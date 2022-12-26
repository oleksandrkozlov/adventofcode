#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <iterator>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Valley = std::vector<std::string>;

struct Point {
    auto operator<=>(const Point& point) const = default;
    std::int64_t x{};
    std::int64_t y{};
};

template<>
struct std::hash<Point> {
    [[nodiscard]] auto operator()(const Point& p) const -> std::size_t
    {
        auto result = std::size_t{};
        boost::hash_combine(result, std::hash<std::int64_t>{}(p.x));
        boost::hash_combine(result, std::hash<std::int64_t>{}(p.y));
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

struct Blizzard {
    auto operator<=>(const Blizzard& blizzard) const = default;
    Point point{};
    char direction = '\0';
};

template<>
struct std::hash<Blizzard> {
    [[nodiscard]] auto operator()(const Blizzard& b) const -> std::size_t
    {
        auto result = std::size_t{};
        boost::hash_combine(result, std::hash<Point>{}(b.point));
        boost::hash_combine(result, std::hash<char>{}(b.direction));
        return result;
    }
};

using Blizzards = std::vector<Blizzard>;

template<>
struct std::hash<Blizzards> {
    [[nodiscard]] auto operator()(const Blizzards& blizzards) const -> std::size_t
    {
        auto result = std::size_t{};
        for (const auto& blizzard : blizzards) {
            boost::hash_combine(result, std::hash<Blizzard>{}(blizzard));
        }
        return result;
    }
};

using AllBlizzards = std::unordered_map<std::int64_t, Blizzards>;
using State = std::tuple<Blizzards, Point, std::int64_t>;
using States = std::unordered_set<State>;

template<>
struct std::hash<State> {
    [[nodiscard]] auto operator()(const State& state) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto& [blizzards, point, time_left] = state;
        boost::hash_combine(result, std::hash<Blizzards>{}(blizzards));
        boost::hash_combine(result, std::hash<Point>{}(point));
        boost::hash_combine(result, std::hash<std::int64_t>{}(time_left));
        return result;
    }
};

[[nodiscard]] auto get_valley(const std::string_view input) -> Valley
{
    auto result = Valley{};
    for (const auto row : input | ranges::views::split('\n')) {
        result.push_back(row | ranges::to<std::string>());
    };
    return result;
}

[[nodiscard]] auto get_size(const Valley& valley) -> Point
{
    return {
        static_cast<std::int64_t>(std::size(valley.at(0))),
        static_cast<std::int64_t>(std::size(valley))};
}

[[nodiscard]] auto get_start_finish(const Valley& valley) -> std::pair<Point, Point>
{
    auto result = std::pair<Point, Point>{};
    auto& [start, finish] = result;
    const auto& start_row = *std::begin(valley);
    const auto& finish_row = *std::prev(std::end(valley));
    start.x = static_cast<std::int64_t>(start_row.find_first_of("."));
    finish.x = static_cast<std::int64_t>(finish_row.find_first_of("."));
    finish.y = static_cast<std::int64_t>(std::size(valley) - 1);
    return result;
}

[[nodiscard]] auto get_blizzards(const Valley& valley) -> Blizzards
{
    auto result = Blizzards{};
    for (auto y = 0; y < static_cast<std::int64_t>(std::size(valley)); ++y) {
        for (auto x = 0; x < static_cast<std::int64_t>(std::size(valley.at(y))); ++x) {
            const auto direction = valley.at(y).at(x);
            if (constexpr auto dir = std::array{'<', '>', 'v', '^'};
                ranges::any_of(dir, [&](const auto c) { return c == direction; })) {
                result.emplace_back(Point{x, y}, direction);
            }
        }
    }
    return result;
}

auto move_blizzard(Blizzard& blizzard, const Point& size) -> void
{
    if (blizzard.direction == '>') {
        if ((blizzard.point.x + 1) >= (size.x - 1)) {
            blizzard.point.x = 1;
        } else {
            ++blizzard.point.x;
        }
    } else if (blizzard.direction == '<') {
        if (blizzard.point.x - 1 <= 0) {
            blizzard.point.x = size.x - 2;
        } else {
            --blizzard.point.x;
        }
    } else if (blizzard.direction == '^') {
        if ((blizzard.point.y - 1) <= 0) {
            blizzard.point.y = size.y - 2;
        } else {
            --blizzard.point.y;
        }
    } else {
        assert(blizzard.direction == 'v');
        if ((blizzard.point.y + 1) >= (size.y - 1)) {
            blizzard.point.y = 1;
        } else {
            ++blizzard.point.y;
        }
    }
}

auto move_blizzards(Blizzards& blizzards, const Point& size) -> void
{
    for (auto& blizzard : blizzards) {
        move_blizzard(blizzard, size);
    }
}

[[nodiscard]] auto has_blizzard(const Blizzards& blizzards, const Point& point) -> bool
{
    return (ranges::find_if(blizzards, [&](const auto& blizzard) {
                return blizzard.point == point;
            }) != std::cend(blizzards));
}

[[nodiscard]] auto is_wall(
    const Point& point,
    const Point& size,
    const Point& start,
    const Point& finish) -> bool
{
    return ((point != start) && (point != finish)) &&
        (((point.y <= 0) || (point.y >= size.y - 1)) ||
         ((point.x <= 0) || (point.x >= size.x - 1)));
}

[[maybe_unused]] auto print_valley(
    Blizzards blizzards,
    const Point& size,
    const Point& point,
    const Point& start,
    const Point& finish) -> void
{
    auto valley = Valley{};
    for (auto y = 0; y < size.y; ++y) {
        valley.emplace_back(size.x, '.');
    }
    for (auto y = 0; y < size.y; ++y) {
        valley.at(y).at(0) = '#';
        valley.at(y).at(size.x - 1) = '#';
    }
    for (auto x = 0; x < size.x; ++x) {
        valley.at(0).at(x) = '#';
        valley.at(size.y - 1).at(x) = '#';
    }
    valley.at(start.y).at(start.x) = '.';
    valley.at(finish.y).at(finish.x) = '.';
    for (const auto blizzard : blizzards) {
        auto direction = blizzard.direction;
        auto count = std::int64_t{};
        for (const auto b : blizzards) {
            if (b.point == blizzard.point) {
                ++count;
            }
        }
        if (count > 1) {
            direction = std::to_string(count).at(0);
        }
        valley.at(blizzard.point.y).at(blizzard.point.x) = direction;
    }
    valley.at(point.y).at(point.x) = 'E';
    for (const auto& row : valley) {
        fmt::print("{}\n", row);
    }
    fmt::print("\n");
}

[[nodiscard]] auto get_min_minutes(
    Blizzards& blizzards,
    const Point& size,
    const Point& start,
    const Point& finish) -> std::int64_t
{
    auto all_blizzards = AllBlizzards{};
    auto states = States{};
    auto time_left = std::int64_t{};
    auto point = start;
    auto queue = std::queue<std::tuple<Blizzards, Point, std::int64_t>>{};
    queue.push({blizzards, point, std::int64_t{}});
    while (!std::empty(queue)) {
        const auto state = queue.front();
        queue.pop();
        blizzards = std::get<Blizzards>(state);
        point = std::get<Point>(state);
        time_left = std::get<std::int64_t>(state);
        if (point == finish) {
            return time_left;
        }
        if (states.contains(state)) {
            continue;
        }
        states.emplace(state);
        if (all_blizzards.contains(time_left + 1)) {
            blizzards = all_blizzards.at(time_left + 1);
        } else {
            move_blizzards(blizzards, size);
            all_blizzards.emplace(time_left + 1, blizzards);
        }
        const auto dirs = std::array<Point, 5>{{{-1, 0}, {0, 0}, {0, -1}, {0, 1}, {1, 0}}};
        for (const auto& p : dirs) {
            const auto next_point = Point{point.x + p.x, point.y + p.y};
            if (not is_wall(next_point, size, start, finish) &&
                not has_blizzard(blizzards, next_point)) {
                queue.push({blizzards, next_point, time_left + 1});
            }
        }
    }
    return -1;
}

auto main() -> int
{
    const auto valley = get_valley(Input);
    const auto start_finish = get_start_finish(valley);
    const auto& [start, finish] = start_finish;
    const auto size = get_size(valley);
    auto blizzards = get_blizzards(valley);
    const auto result1 = get_min_minutes(blizzards, size, start, finish);
    fmt::print("{}\n", result1);
    assert(result1 == 266);
    auto result2 = get_min_minutes(blizzards, size, finish, start);
    auto result3 = get_min_minutes(blizzards, size, start, finish);
    result2 += result1 + result3;
    fmt::print("{}\n", result2);
    assert(result2 == 853);
}
