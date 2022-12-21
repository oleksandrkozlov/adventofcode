#include "input.h"

#include <range/v3/all.hpp>

#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

struct point {
    int x = 0;
    int y = 0;
};

[[nodiscard]] auto get_min_max(const std::vector<std::pair<std::string, int>>& moves)
    -> std::pair<point, point>
{
    auto x = 0;
    auto y = 0;
    auto max = point{};
    auto min = point{INT_MAX, INT_MAX};

    for (const auto& [direction, distance] : moves) {
        if (direction == "R") {
            x += distance;
            max.x = std::max(x, max.x);
        }
        if (direction == "L") {
            x -= distance;
            min.x = std::min(x, min.x);
        }
        if (direction == "U") {
            y += distance;
            max.y = std::max(y, max.y);
        }
        if (direction == "D") {
            y -= distance;
            min.y = std::min(y, min.y);
        }
    }
    if (min.x < 0) {
        max.x += (min.x * -1);
    }
    if (min.y < 0) {
        max.y += (min.y * -1);
    }
    return {min, max};
}

[[nodiscard]] auto create_result(const point& max) -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    for (auto y = 0; y < max.y + 1; ++y) {
        result.push_back(std::string(max.x + 1, ' '));
    }
    return result;
}

[[nodiscard]] auto create_moves(const std::string_view input)
    -> std::vector<std::pair<std::string, int>>
{
    auto result = std::vector<std::pair<std::string, int>>{};
    for (const auto range : input | ranges::views::split('\n')) {
        auto line = range | ranges::views::split(' ');
        const auto direction = *ranges::begin(line) | ranges::to<std::string>();
        const auto step = std::stoi(*ranges::next(ranges::begin(line)) | ranges::to<std::string>());
        result.emplace_back(direction, step);
    }
    return result;
}

[[nodiscard]] auto get_head(const point& min) -> point
{
    auto result = point{};
    if (min.x < 0) {
        result.x = (min.x * -1);
    }
    if (min.y < 0) {
        result.y = (min.y * -1);
    }
    return result;
}

auto update_tail(const point& head, point& tail) -> void
{
    const auto diff = point{head.x - tail.x, head.y - tail.y};
    if (diff.x == 2 && (head.y == tail.y)) {
        ++tail.x;
    } else if (diff.x == -2 && (head.y == tail.y)) {
        --tail.x;
    } else if (diff.y == 2 && (head.x == tail.x)) {
        ++tail.y;
    } else if (diff.y == -2 && (head.x == tail.x)) {
        --tail.y;
    } else if (diff.x == 2 && !(head.y == tail.y)) {
        ++tail.x;
        if (diff.y > 0) {
            ++tail.y;
        } else {
            --tail.y;
        }
    } else if (diff.x == -2 && !(head.y == tail.y)) {
        --tail.x;
        if (diff.y > 0) {
            ++tail.y;
        } else {
            --tail.y;
        }
    } else if (diff.y == 2 && !(head.x == tail.x)) {
        ++tail.y;
        if (diff.x > 0) {
            ++tail.x;
        } else {
            --tail.x;
        }
    } else if (diff.y == -2 && !(head.x == tail.x)) {
        --tail.y;
        if (diff.x > 0) {
            ++tail.x;
        } else {
            --tail.x;
        }
    }
}

auto mark_visited(std::vector<std::string>& result, const auto& point) -> void
{
    result.at(point.y).at(point.x) = '#';
}

[[nodiscard]] auto get_positions(const std::string_view input, const std::size_t rope_size) -> int
{
    const auto moves = create_moves(input);
    const auto [min, max] = get_min_max(moves);
    auto result = create_result(max);
    auto tails = std::vector<point>{rope_size, get_head(min)};
    auto& head = tails.at(0);

    for (const auto& [direction, distance] : moves) {
        if (direction == "R") {
            const auto to = head.x + distance;
            for (; head.x < to; ++head.x) {
                for (auto i = std::size_t{1}; i < tails.size(); ++i) {
                    update_tail(tails.at(i - 1), tails.at(i));
                    if (i == rope_size - 1) {
                        mark_visited(result, tails.at(i));
                    }
                }
            }
        } else if (direction == "L") {
            const auto to = head.x - distance;
            for (; head.x > to; --head.x) {
                for (auto i = std::size_t{1}; i < tails.size(); ++i) {
                    update_tail(tails.at(i - 1), tails.at(i));
                    if (i == rope_size - 1) {
                        mark_visited(result, tails.at(i));
                    }
                }
            }
        } else if (direction == "U") {
            const auto to = head.y + distance;
            for (; head.y < to; ++head.y) {
                for (auto i = std::size_t{1}; i < tails.size(); ++i) {
                    update_tail(tails.at(i - 1), tails.at(i));
                    if (i == rope_size - 1) {
                        mark_visited(result, tails.at(i));
                    }
                }
            }
        } else if (direction == "D") {
            const auto to = head.y - distance;
            for (; head.y > to; --head.y) {
                for (auto i = std::size_t{1}; i < tails.size(); ++i) {
                    update_tail(tails.at(i - 1), tails.at(i));
                    if (i == rope_size - 1) {
                        mark_visited(result, tails.at(i));
                    }
                }
            }
        }
    }
    for (auto i = std::size_t{1}; i < tails.size(); ++i) {
        update_tail(tails.at(i - 1), tails.at(i));
        if (i == rope_size - 1) {
            mark_visited(result, tails.at(i));
        }
    }
    auto rv = 0;
    for (const auto& xy : result) {
        for (const auto c : xy) {
            if (c == '#') {
                ++rv;
            }
        }
    }
    return rv;
}

int main()
{
    const auto result1 = get_positions(Input, 2);
    const auto result2 = get_positions(Input, 10);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 6391);
    assert(result2 == 2593);
}
