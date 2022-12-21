#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <iostream>
#include <ostream>
#include <queue>
#include <string>
#include <string_view>

struct Point {
    int y = 0;
    int x = 0;
    int dist = 0;
};

[[nodiscard]] auto get_min_distance(
    std::vector<std::string>& grid,
    const Point& source,
    const Point& destination) -> int
{
    auto visited = std::vector<std::vector<int>>{};
    const auto size_y = grid.size();
    const auto size_x = grid.at(0).size();
    for (auto y = std::size_t{}; y < size_y; ++y) {
        visited.push_back(std::vector(size_x, 0));
    }
    [[maybe_unused]] const auto print = [&] {
        std::system("clear");
        for (auto i = std::size_t{}; i < visited.size(); ++i) {
            for (auto j = std::size_t{}; j < visited.at(i).size(); ++j) {
                if (visited.at(i).at(j) == 1) {
                    std::cout << "\033[31m";
                }
                std::cout << grid.at(i).at(j) << "\033[39m";
            }
            std::cout << '\n';
        }
    };
    auto q = std::queue<Point>{};
    q.push(source);
    visited.at(source.y).at(source.x) = 1;
    while (!q.empty()) {
        // print();
        auto p = q.front();
        q.pop();
        if ((p.y == destination.y) && (p.x == destination.x)) {
            return p.dist;
        }
        const auto c = grid.at(p.y).at(p.x);
        if (const auto y = p.y - 1; y >= 0 && !visited.at(y).at(p.x)) {
            if (grid.at(y).at(p.x) - c <= 1) {
                q.push({y, p.x, p.dist + 1});
                visited.at(y).at(p.x) = 1;
            }
        }
        if (const auto y = p.y + 1; y < static_cast<int>(size_y) && !visited.at(y).at(p.x)) {
            if (grid.at(y).at(p.x) - c <= 1) {
                q.push({y, p.x, p.dist + 1});
                visited.at(y).at(p.x) = 1;
            }
        }
        if (const auto x = p.x - 1; x >= 0 && !visited.at(p.y).at(x)) {
            if (grid.at(p.y).at(x) - c <= 1) {
                q.push({p.y, x, p.dist + 1});
                visited.at(p.y).at(x) = 1;
            }
        }
        if (const auto x = p.x + 1; x < static_cast<int>(size_x) && !visited.at(p.y).at(x)) {
            if (grid.at(p.y).at(x) - c <= 1) {
                q.push({p.y, x, p.dist + 1});
                visited.at(p.y).at(x) = 1;
            }
        }
    }
    return -1;
}

auto main() -> int
{
    auto grid = std::vector<std::string>{};
    for (const auto range : Input | ranges::views::split('\n')) {
        grid.push_back(range | ranges::to<std::string>());
    }
    auto sources = std::vector<Point>{};
    const auto size_y = grid.size();
    const auto size_x = grid.at(0).size();
    auto source = Point{};
    auto destination = Point{};
    for (auto y = std::size_t{}; y < size_y; ++y) {
        for (auto x = std::size_t{}; x < size_x; ++x) {
            if (grid.at(y).at(x) == 'S') {
                grid.at(y).at(x) = 'a';
                source.x = static_cast<int>(x);
                source.y = static_cast<int>(y);
                sources.push_back({static_cast<int>(y), static_cast<int>(x), 0});
            }
            if (grid.at(y).at(x) == 'a') {
                sources.push_back({static_cast<int>(y), static_cast<int>(x), 0});
            }
            if (grid.at(y).at(x) == 'E') {
                destination.x = static_cast<int>(x);
                destination.y = static_cast<int>(y);
                grid.at(y).at(x) = 'z';
            }
        }
    }
    auto results = std::vector<int>{};
    for (const auto& src : sources) {
        if (const auto result = get_min_distance(grid, src, destination); result != -1) {
            results.push_back(result);
        }
    }
    std::sort(results.begin(), results.end());
    const auto result1 = get_min_distance(grid, source, destination);
    const auto result2 = results.at(0);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 437);
    assert(result2 == 430);
}
