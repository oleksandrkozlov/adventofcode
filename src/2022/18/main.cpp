#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <array>
#include <cassert>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

struct point {
    int x = 0;
    int y = 0;
    int z = 0;
    auto operator<=>(const point& p) const = default;
};

template<>
struct std::hash<point> {
    [[nodiscard]] auto operator()(const point& p) const -> std::size_t
    {
        auto result = std::size_t{};
        boost::hash_combine(result, boost::hash_value(p.x));
        boost::hash_combine(result, boost::hash_value(p.y));
        boost::hash_combine(result, boost::hash_value(p.z));
        return result;
    }
};

[[nodiscard]] auto parse_input(const std::string_view input) -> std::unordered_set<point>
{
    auto result = std::unordered_set<point>{};
    for (const auto range : input | ranges::views::split('\n')) {
        const auto point = range | ranges::views::split(',');
        auto it = ranges::cbegin(point);
        const auto x = std::stoi(*it | ranges::to<std::string>());
        it = ranges::next(it);
        const auto y = std::stoi(*it | ranges::to<std::string>());
        it = ranges::next(it);
        const auto z = std::stoi(*it | ranges::to<std::string>());
        result.emplace(x, y, z);
    }
    return result;
}

[[nodiscard]] auto contains(const std::unordered_set<point>& points, point p, const auto& dir)
    -> bool
{
    auto v = std::unordered_set<point>{};
    auto q = std::queue<point>{};
    q.push(p);
    while (!q.empty()) {
        p = q.front();
        q.pop();
        if (!points.contains(p) && !v.contains(p)) {
            v.insert(p);
            const auto arbitrary_size = 1024;
            if (std::size(v) > arbitrary_size) {
                return true;
            }
            for (const auto& d : dir) {
                q.push({p.x + d.x, p.y + d.y, p.z + d.z});
            }
        }
    }
    return false;
}

[[nodiscard]] auto get_surface_area(const std::unordered_set<point>& points, const int part) -> int
{
    auto result = 0;
    const auto dir = std::array{
        // clang-format off
        point{1, 0, 0}, point{-1,  0,  0},
        point{0, 1, 0}, point{ 0, -1,  0},
        point{0, 0, 1}, point{ 0,  0, -1},
        // clang-format on
    };
    for (const auto& p : points) {
        for (const auto& d : dir) {
            const auto pp = point{p.x + d.x, p.y + d.y, p.z + d.z};
            if (part == 1) {
                if (!points.contains(pp)) {
                    ++result;
                }
            } else if (part == 2) {
                if (contains(points, pp, dir)) {
                    ++result;
                }
            }
        }
    }
    return result;
}

auto main() -> int
{
    const auto points = parse_input(Input);
    const auto result1 = get_surface_area(points, 1);
    const auto result2 = get_surface_area(points, 2);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 3346);
    assert(result2 == 1980);
}
