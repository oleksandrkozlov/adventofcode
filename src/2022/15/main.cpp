#include "input.h"

#include <gmpxx.h>
#include <range/v3/all.hpp>

#include <array>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

struct point {
    int x = 0;
    int y = 0;
    auto operator<=>(const point&) const = default;
};

template<>
struct std::hash<point> {
    std::size_t operator()(point const& p) const noexcept
    {
        const auto h1 = std::hash<std::string>{}(std::to_string(p.x));
        const auto h2 = std::hash<std::string>{}(std::to_string(p.y));
        return h1 ^ (h2 << 1);
    }
};

struct points {
    point s;
    point b;
};

[[nodiscard]] auto get_coordinates(const std::string_view input)
    -> std::vector<std::pair<point, point>>
{
    using namespace std::literals;
    auto result = std::vector<std::pair<point, point>>{};
    for (const auto range : input | ranges::view::split('\n')) {
        const auto values = range | ranges::view::split(ranges::view::c_str(", "));
        const auto first = *values.begin() | ranges::to<std::string>();
        const auto second = *ranges::next(values.begin(), 1) | ranges::to<std::string>();
        const auto third = *ranges::next(values.begin(), 2) | ranges::to<std::string>();
        const auto sensor = point{
            std::stoi(first.substr(std::size("Sensor at x="sv))),
            std::stoi(second.substr(std::size("y=") - 1))};
        const auto beacon = point{
            std::stoi(second.substr(second.find("x=") + 2)),
            std::stoi(third.substr(std::size("y=") - 1))};
        result.emplace_back(sensor, beacon);
    }
    return result;
};

[[nodiscard]] constexpr auto get_distance(const point& sensor, const point& beacon) -> int
{
    return std::abs(beacon.x - sensor.x) + std::abs(beacon.y - sensor.y);
}

[[nodiscard]] constexpr auto is_point_inside_rhombus(
    const point& sensor,
    const point& beacon,
    const point& point) -> bool
{
    return get_distance(sensor, point) <= get_distance(sensor, beacon);
}

[[nodiscard]] auto get_frequency(const auto& coordinates, const std::vector<point>& points)
    -> mpz_class
{
    for (const auto& point : points) {
        if (ranges::none_of(coordinates, [&](const auto& values) {
                const auto& [sensor, beacon] = values;
                return is_point_inside_rhombus(sensor, beacon, point);
            })) {
            return mpz_class{4000000} * point.x + point.y;
        }
    }
    return -1;
}

[[nodiscard]] auto to_point(const point& p, const point& sensor, const int distance) -> point
{
    return point{p.x - distance + sensor.x, p.y - distance + sensor.y};
}

auto cross_map(
    std::unordered_set<point>& set,
    const point& sensor,
    const int distance,
    const int row)
{
    // right down
    for (auto yy = distance, y = yy, i = 0; y < (distance * 2) + 1; ++y, ++i) {
        auto p = to_point(point{0, y}, sensor, distance);
        if (p.y != row) {
            continue;
        }
        for (auto xx = distance, x = xx; x < (distance * 2) + 1 - i; ++x) {
            p = to_point(point{x, y}, sensor, distance);
            set.emplace(p);
        }
    }
    // right top
    for (auto yy = distance, y = yy, i = 0; y >= 0; --y, ++i) {
        auto p = to_point(point{0, y}, sensor, distance);
        if (p.y != row) {
            continue;
        }
        for (auto xx = distance, x = xx; x < (distance * 2) + 1 - i; ++x) {
            p = to_point(point{x, y}, sensor, distance);
            set.emplace(p);
        }
    }
    // left down
    for (auto yy = distance, y = yy, i = 0; y < (distance * 2) + 1; ++y, ++i) {
        auto p = to_point(point{0, y}, sensor, distance);
        if (p.y != row) {
            continue;
        }
        for (auto xx = distance, x = xx; x >= i; --x) {
            p = to_point(point{x, y}, sensor, distance);
            set.emplace(p);
        }
    }
    // left up
    for (auto yy = distance, y = yy, i = 0; y >= 0; --y, ++i) {
        auto p = to_point(point{0, y}, sensor, distance);
        if (p.y != row) {
            continue;
        }
        for (auto xx = distance, x = xx; x >= i; --x) {
            p = to_point(point{x, y}, sensor, distance);
            set.emplace(p);
        }
    }
}

[[nodiscard]] auto get_points(const point& sensor, const int distance, const int max)
    -> std::vector<point>
{
    auto result = std::vector<point>{};
    for (auto i = sensor.x + distance, j = sensor.y; i > sensor.x; --i, --j) {
        if (i >= 0 && i <= max && j >= 0 && j <= max) {
            result.emplace_back(i, j);
        }
    }
    for (auto i = sensor.x, j = sensor.y - distance; i > sensor.x - distance; --i, ++j) {
        if (i >= 0 && i <= max && j >= 0 && j <= max) {
            result.emplace_back(i, j);
        }
    }
    for (auto i = sensor.x - distance, j = sensor.y; i < sensor.x; ++i, ++j) {
        if (i >= 0 && i <= max && j >= 0 && j <= max) {
            result.emplace_back(i, j);
        }
    }
    for (auto i = sensor.x, j = sensor.y + distance; i < sensor.x + distance; ++i, --j) {
        if (i >= 0 && i <= max && j >= 0 && j <= max) {
            result.emplace_back(i, j);
        }
    }
    return result;
}

[[nodiscard]] auto get_all_points(const auto& coordinates, const auto max) -> std::vector<point>
{
    auto result = std::vector<point>{};
    for (const auto& [sensor, beacon] : coordinates) {
        const auto points = get_points(sensor, get_distance(sensor, beacon) + 1, max);
        result.insert(std::end(result), std::cbegin(points), std::cend(points));
    }
    return result;
}

[[nodiscard]] auto get_positions(const std::vector<std::pair<point, point>>& coordinates)
    -> std::size_t
{
    auto result = std::unordered_set<point>{};
    for (const auto& [sensor, beacon] : coordinates) {
        const auto distance = get_distance(sensor, beacon);
        cross_map(result, sensor, distance, 2000000);
    }
    for (const auto& [sensor, beacon] : coordinates) {
        if (result.contains(sensor)) {
            result.erase(sensor);
        }
        if (result.contains(beacon)) {
            result.erase(beacon);
        }
    }
    return std::size(result);
}

auto main() -> int
{
    const auto coordinates = get_coordinates(Input);
    const auto result1 = get_positions(coordinates);
    const auto result2 = get_frequency(coordinates, get_all_points(coordinates, 4000000));
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 5299855);
    assert(result2 == 13615843289729);
}
