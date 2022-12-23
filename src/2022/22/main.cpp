#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

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

enum class Turn {
    Stop,
    Right,
    Left,
};

template<>
struct fmt::formatter<Turn> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const Turn turn, FormatContext& ctx)
    {
        return fmt::formatter<std::string_view>::format(
            [&] {
                if (turn == Turn::Right) {
                    return "right";
                }
                if (turn == Turn::Left) {
                    return "left";
                }
                assert(turn == Turn::Stop);
                {
                    return "stop";
                }
            }(),
            ctx);
    }
};

[[nodiscard]] constexpr auto to_turn(const std::string_view c) noexcept -> Turn
{
    if (c == "L") {
        return Turn::Left;
    }
    if (c == "R") {
        return Turn::Right;
    }
    assert(c == "");
    {
        return Turn::Stop;
    }
}

enum class Direction {
    Right,
    Left,
    Up,
    Down,
};

template<>
struct fmt::formatter<Direction> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const Direction direction, FormatContext& ctx)
    {
        return fmt::formatter<std::string_view>::format(
            [&] {
                if (direction == Direction::Right) {
                    return "right";
                }
                if (direction == Direction::Left) {
                    return "left";
                }
                if (direction == Direction::Up) {
                    return "up";
                }
                assert(direction == Direction::Down);
                {
                    return "down";
                }
            }(),
            ctx);
    }
};

constexpr const auto make_turn(const Turn turn, Direction& direction) noexcept -> void
{
    if (turn == Turn::Stop) {
        return;
    }
    if (direction == Direction::Right) {
        if (turn == Turn::Right) {
            direction = Direction::Down;
            return;
        }
        assert(turn == Turn::Left);
        {
            direction = Direction::Up;
            return;
        }
    }
    if (direction == Direction::Left) {
        if (turn == Turn::Right) {
            direction = Direction::Up;
            return;
        }
        assert(turn == Turn::Left);
        {
            direction = Direction::Down;
            return;
        }
    }
    if (direction == Direction::Up) {
        if (turn == Turn::Right) {
            direction = Direction::Right;
            return;
        }
        assert(turn == Turn::Left);
        {
            direction = Direction::Left;
            return;
        }
    }
    assert(direction == Direction::Down);
    {
        if (turn == Turn::Right) {
            direction = Direction::Left;
            return;
        }
        assert(turn == Turn::Left);
        {
            direction = Direction::Right;
            return;
        }
    }
}

struct Move {
    explicit Move(const int s, const Turn t = Turn::Stop)
        : moves{s}
        , turn{t}
    {
    }

    auto operator<=>(const Move& point) const = default;

    int moves{};
    Turn turn = Turn::Stop;
};

template<>
struct fmt::formatter<Move> : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const Move& m, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{} -> {}", m.moves, m.turn);
    }
};

enum class Tile {
    Void,
    Wall,
    Road,
    PathRight,
    PathLeft,
    PathUp,
    PathDown,
};

[[nodiscard]] constexpr auto to_tile(const char c) noexcept -> Tile
{
    if (c == '.') {
        return Tile::Road;
    }
    if (c == '#') {
        return Tile::Wall;
    }
    if (c == ' ') {
        return Tile::Void;
    }
    if (c == '>') {
        return Tile::PathRight;
    }
    if (c == '<') {
        return Tile::PathLeft;
    }
    if (c == '^') {
        return Tile::PathUp;
    }
    assert(c == 'V');
    {
        return Tile::PathDown;
    }
}

[[nodiscard]] constexpr auto from_tile(const Tile tile) noexcept -> char
{
    if (tile == Tile::Road) {
        return '.';
    }
    if (tile == Tile::Wall) {
        return '#';
    }
    if (tile == Tile::Void) {
        return ' ';
    }
    if (tile == Tile::PathRight) {
        return '>';
    }
    if (tile == Tile::PathLeft) {
        return '<';
    }
    if (tile == Tile::PathDown) {
        return 'V';
    }
    assert(tile == Tile::PathUp);
    {
        return '^';
    }
}

[[nodiscard]] constexpr auto get_tile_path_direction(const Direction direction) noexcept -> Tile
{
    if (direction == Direction::Right) {
        return Tile::PathRight;
    }
    if (direction == Direction::Left) {
        return Tile::PathLeft;
    }
    if (direction == Direction::Down) {
        return Tile::PathDown;
    }
    assert(direction == Direction::Up);
    {
        return Tile::PathUp;
    }
}

using Map = std::unordered_map<Point, Tile>;
using Route = std::vector<Move>;

[[nodiscard]] auto is_tile_void(const Point& p, const Map& map) -> bool
{
    return !map.contains(p) || map.at(p) == Tile::Void;
}

[[nodiscard]] auto is_tile_wall(const Point& p, const Map& map) -> bool
{
    assert(map.contains(p));
    return map.at(p) == Tile::Wall;
}

[[nodiscard]] auto is_tile_road(const Point& p, const Map& map) -> bool
{
    assert(map.contains(p));
    return (map.at(p) == Tile::Road) || (map.at(p) == Tile::PathRight) ||
        (map.at(p) == Tile::PathLeft) || (map.at(p) == Tile::PathUp) ||
        (map.at(p) == Tile::PathDown);
}

auto print_map(const Map& map, const Point& size, const Point& point) -> void
{
    for (auto j = 0; j < size.y; ++j) {
        fmt::print("{} ", j);
        for (auto i = 0; i < size.x; ++i) {
            const auto p = Point{i, j};
            auto& tile = map.at(p);
            const auto c = (p == point) ? '@' : from_tile(tile);
            if (tile == Tile::Void || tile == Tile::Road || tile == Tile::Wall) {
                fmt::print("{}", c);
            } else {
                fmt::print(fmt::fg(fmt::color::light_salmon), "{}", c);
            }
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

[[nodiscard]] auto parse_input(const std::string_view input) -> std::tuple<Map, Point, Route>
{
    auto result = std::tuple<Map, Point, Route>{};
    auto& [map, p, route] = result;
    const auto range = input | ranges::views::split(ranges::views::c_str("\n\n"));
    const auto begin = std::cbegin(range);
    const auto route_str = (*std::next(begin) | ranges::to<std::string>());
    const auto is_step = [](const auto c) { return c == 'L' || c == 'R'; };
    const auto is_turn = [](const auto c) { return c != 'L' && c != 'R'; };
    for (const auto moves_str : route_str | ranges::views::split_when(is_step)) {
        route.emplace_back(std::stoi(moves_str | ranges::to<std::string>()));
    }
    auto k = 0;
    for (const auto turn_str : route_str | ranges::views::split_when(is_turn)) {
        const auto turn = to_turn(turn_str | ranges::to<std::string>());
        if (turn != Turn::Stop) {
            route.at(k).turn = turn;
            ++k;
        }
    }
    for (const auto row : *begin | ranges::views::split('\n')) {
        const auto row_str = row | ranges::to<std::string>();
        for (p.x = 0; p.x < static_cast<int>(std::size(row_str)); ++p.x) {
            map.emplace(Point{p.x, p.y}, to_tile(row_str.at(p.x)));
        }
        ++p.y;
    }
    return result;
}

[[nodiscard]] auto get_start_point(const Map& map) -> Point
{
    auto result = Point{INT_MAX, INT_MAX};
    auto road_points = map | ranges::views::filter([](const auto& kv) {
                           const auto& [_, tile] = kv;
                           return tile == Tile::Road;
                       });
    for (const auto& [p, _] : road_points) {
        result.y = std::min(result.y, p.y);
    }
    for (const auto& [p, _] : road_points) {
        if (p.y == result.y) {
            result.x = std::min(result.x, p.x);
        }
    }
    return result;
}

[[nodiscard]] auto get_next_point_after_void_on_flat(
    const Map& map,
    Point point,
    const Direction direction) -> Point
{
    auto non_void_points = map | ranges::views::filter([](const auto& kv) {
                               const auto& [_, tile] = kv;
                               return tile != Tile::Void;
                           });
    if (direction == Direction::Right) {
        point.x = INT_MAX;
        for (const auto& [p, _] : non_void_points) {
            if (p.y == point.y) {
                point.x = std::min(point.x, p.x);
            }
        }
    } else if (direction == Direction::Left) {
        point.x = INT_MIN;
        for (const auto& [p, _] : non_void_points) {
            if (p.y == point.y) {
                point.x = std::max(point.x, p.x);
            }
        }
    } else if (direction == Direction::Down) {
        point.y = INT_MAX;
        for (const auto& [p, _] : non_void_points) {
            if (p.x == point.x) {
                point.y = std::min(point.y, p.y);
            }
        }
    } else {
        assert(direction == Direction::Up);
        point.y = INT_MIN;
        for (const auto& [p, _] : non_void_points) {
            if (p.x == point.x) {
                point.y = std::max(point.y, p.y);
            }
        }
    }
    return point;
}

//          ┌─────────┬────────┐
//          │    D1   │   E1   │
//          │         │        │
//          │C1       │      G1│
//          │         │   F2   │
//          ├─────────┼────────┘
//          │         │
//          │B1     F1│
//          │         │
//          │         │
// ┌────────┼─────────┤
// │   B2   │         │
// │        │       G2│
// │C2      │         │
// │        │    A1   │
// ├────────┼─────────┘
// │        │
// │D2    A2│
// │        │
// │   E2   │
// └────────┘

[[nodiscard]] auto get_next_point_after_void_on_cube(Point point, Direction direction)
    -> std::pair<Point, Direction>
{
    const auto L1 = 50;
    const auto L2 = 100;
    const auto L3 = 150;
    const auto L4 = 200;

    auto& [x, y] = point;
    if (y == L3 && (x >= L1 && x < L2) && (direction == Direction::Down)) { // A1
        y = x + L2;
        x = L1 - 1;
        direction = Direction::Left;
    } else if (x == L1 && (y >= L3 && y < L4) && (direction == Direction::Right)) { // A2
        x = y - L2;
        y = L3 - 1;
        direction = Direction::Up;
    } else if (x == L1 - 1 && (y >= L1 && y < L2) && (direction == Direction::Left)) { // B1
        x = y - L1;
        y = L2;
        direction = Direction::Down;
    } else if (y == L2 - 1 && (x >= 0 && x < L1) && (direction == Direction::Up)) { // B2
        y = x + L1;
        x = L1;
        direction = Direction::Right;
    } else if (x == L1 - 1 && (y >= 0 && y < L1) && (direction == Direction::Left)) { // C1
        y = (L3 - 1) - (5 / 3) * y;
        x = 0;
        direction = Direction::Right;
    } else if (x == -1 && (y >= L2 && y < L3) && (direction == Direction::Left)) { // C2
        y = (L3 - 1) - (5 / 3) * y;
        x = L1;
        direction = Direction::Right;
    } else if (y == -1 && (x >= L1 && x < L2) && (direction == Direction::Up)) { // D1
        y = x + L2;
        x = 0;
        direction = Direction::Right;
    } else if (x == -1 && (y >= L3 && y < L4) && (direction == Direction::Left)) { // D2
        x = y - L2;
        y = 0;
        direction = Direction::Down;
    } else if (y == -1 && (x >= L2 && x < L3) && (direction == Direction::Up)) { // E1
        x = x - L2;
        y = L4 - 1;
        direction = Direction::Up;
    } else if (y == L4 && (x >= 0 && x < L1) && (direction == Direction::Down)) { // E2
        x = x + L2;
        y = 0;
        direction = Direction::Down;
    } else if (x == L2 && (y >= L1 && y < L2) && (direction == Direction::Right)) { // F1
        x = y + L1;
        y = L1 - 1;
        direction = Direction::Up;
    } else if (y == L1 && (x >= L2 && x < L3) && (direction == Direction::Down)) { // F2
        y = x - L1;
        x = L2 - 1;
        direction = Direction::Left;
    } else if (x == L3 && (y >= 0 && y < L1) && (direction == Direction::Right)) { // G1
        x = L2 - 1;
        y = (L3 - 1) - (5 / 3) * y;
        direction = Direction::Left;
    } else if (x == L2 && (y >= L2 && y < L3) && (direction == Direction::Right)) { // G2
        x = L3 - 1;
        y = (L3 - 1) - (5 / 3) * y;
        direction = Direction::Left;
    } else {
        assert(false);
    }
    return {point, direction};
}

auto get_next_point_after_void(
    const int part,
    const Map& map,
    const Point& point,
    const Direction direction) -> std::pair<Point, Direction>
{
    if (part == 1) {
        return {get_next_point_after_void_on_flat(map, point, direction), direction};
    }
    assert(part == 2);
    {
        return get_next_point_after_void_on_cube(point, direction);
    }
}

[[nodiscard]] constexpr auto get_direction_points(const Direction direction) noexcept -> int
{
    if (direction == Direction::Right) {
        return 0;
    }
    if (direction == Direction::Down) {
        return 1;
    }
    if (direction == Direction::Left) {
        return 2;
    }
    assert(direction == Direction::Up);
    {
        return 3;
    }
}

[[nodiscard]] auto get_next_point(const Point& p, const Direction direction) -> Point
{
    if (direction == Direction::Right) {
        return {p.x + 1, p.y};
    }
    if (direction == Direction::Left) {
        return {p.x - 1, p.y};
    }
    if (direction == Direction::Down) {
        return {p.x, p.y + 1};
    }
    assert(direction == Direction::Up);
    {
        return {p.x, p.y - 1};
    }
}

auto make_moves(
    const int part,
    Map& map,
    [[maybe_unused]] const Point& size,
    const int moves,
    Point& point,
    Direction& direction) -> void
{
    for (auto i = 0; i < moves; ++i) {
        if (const auto next_point = get_next_point(point, direction);
            is_tile_void(next_point, map)) {
            const auto [point_after_void, direction_after_void] =
                get_next_point_after_void(part, map, next_point, direction);
            if (is_tile_wall(point_after_void, map)) {
                break;
            } else {
                point = point_after_void;
                direction = direction_after_void;
                map.insert_or_assign(point, get_tile_path_direction(direction));
            }
        } else if (is_tile_wall(next_point, map)) {
            break;
        } else {
            assert(is_tile_road(next_point, map));
            point = next_point;
            map.insert_or_assign(point, get_tile_path_direction(direction));
        }
    }
}

auto walk_through_map(
    const int part,
    Map& map,
    const Point& size,
    const Route& route,
    Point& point,
    Direction& direction) -> void
{
    for (const auto& [moves, turn] : route) {
        make_moves(part, map, size, moves, point, direction);
        make_turn(turn, direction);
        map.insert_or_assign(point, get_tile_path_direction(direction));
    }
}

[[nodiscard]] constexpr auto compose_password(
    const Point& point,
    const Direction direction) noexcept -> int
{
    return (1000 * (point.y + 1)) + (4 * (point.x + 1)) + get_direction_points(direction);
}

auto main() -> int
{
    {
        auto [map, size, route] = parse_input(Input);
        auto point = get_start_point(map);
        auto direction = Direction::Right;
        walk_through_map(1, map, size, route, point, direction);
        const auto result = compose_password(point, direction);
        fmt::print("{}\n", result);
        assert(result == 149250);
    }
    {
        auto [map, size, route] = parse_input(Input);
        auto point = get_start_point(map);
        auto direction = Direction::Right;
        walk_through_map(2, map, size, route, point, direction);
        const auto result = compose_password(point, direction);
        fmt::print("{}\n", result);
        assert(result == 12462);
    }
}
