#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <thread>

struct point {
    int x = 0;
    int y = 0;
};

auto main() -> int
{
    const auto get_paths = [](const auto input) {
        auto result = std::vector<std::vector<point>>{};
        for (const auto range : input | ranges::views::split('\n')) {
            auto path = std::vector<point>{};
            for (const auto rng : range | ranges::views::split(ranges::views::c_str(" -> "))) {
                const auto points = rng | ranges::views::split(',');
                const auto x = *ranges::begin(points) | ranges::to<std::string>();
                const auto y = *ranges::next(ranges::begin(points)) | ranges::to<std::string>();
                path.emplace_back(std::stoi(x), std::stoi(y));
            }
            result.push_back(path);
        }
        return result;
    };

    [[maybe_unused]] const auto print_paths = [&](const auto& paths) {
        for (const auto& path : paths) {
            for (const auto& p : path) {
                std::cout << "[" << p.x << ", " << p.y << "] ";
            }
            std::cout << std::endl;
        }
    };

    const auto get_min_max = [&](const auto& paths, const auto part) {
        auto max = point{};
        auto min = point{INT_MAX, INT_MAX};
        for (const auto& path : paths) {
            for (const auto& p : path) {
                max.x = std::max(max.x, p.x);
                max.y = std::max(max.y, p.y);
                min.x = std::min(min.x, p.x);
                min.y = std::min(min.y, p.y);
            }
        }
        ++max.x;
        if (part == 2) {
            max.y += 3;
        } else {
            ++max.y;
        }
        return std::pair{min, max};
    };

    const auto create_empty_map = [](const auto& size) {
        auto result = std::vector<std::string>{};
        for (auto y = 0; y < size.y; ++y) {
            result.push_back(std::string(size.x, '.'));
        }
        return result;
    };

    [[maybe_unused]] const auto print_map = [](const auto& map, const auto& size) {
        std::system("clear");
        std::cout << "    ";
        auto x = 0;
        for (auto i = 0; i < size.x; ++i) {
            if (x == 10) {
                x = 0;
            }
            std::cout << x;
            ++x;
        }
        std::cout << std::endl;
        for (auto y = 0; y < size.y; ++y) {
            if (y < 10) {
                std::cout << "  ";
            } else if (y >= 10 && y < 100) {
                std::cout << " ";
            }
            std::cout << y << " " << map.at(y) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    };

    const auto fill_in_map = [](auto& map, const auto& paths, const auto& min, const auto shift) {
        for (const auto& path : paths) {
            for (auto i = std::size_t{1}; i < path.size(); ++i) {
                auto from = path.at(i - 1);
                auto to = path.at(i);
                from.x -= min.x;
                to.x -= min.x;
                from.x += shift;
                to.x += shift;
                if (from.x == to.x) {
                    if (to.y < from.y) {
                        std::swap(to, from);
                    }
                    for (auto y = from.y; y <= to.y; ++y) {
                        map.at(y).at(to.x) = '#';
                    }
                } else {
                    if (to.x < from.x) {
                        std::swap(to, from);
                    }
                    for (auto x = from.x; x <= to.x; ++x) {
                        map.at(to.y).at(x) = '#';
                    }
                }
            }
        }
        if (shift != 0) {
            map.at((map.size() - 1)) = std::string(map.at(0).size(), '#');
        }
    };

    const auto fall_sand = [&](auto& map, auto sand_x, const auto& size, const auto part) {
        auto result = 0;
        while (true) {
            auto y = 0;
            auto x = sand_x;
            while (true) {
                if ((y + 1) < size.y) {
                    if (map.at(y + 1).at(x) == '.') {
                        ++y;
                        continue;
                    } else {
                        if (x - 1 >= 0) {
                            if (map.at(y + 1).at(x - 1) == '.') {
                                --x;
                                ++y;
                                continue;
                            }
                        } else {
                            if (part == 2) {
                                break;
                            } else {
                                return result;
                            }
                        }
                        if ((x + 1) < size.x) {
                            if (map.at(y + 1).at(x + 1) == '.') {
                                ++x;
                                ++y;
                                continue;
                            }
                        } else {
                            if (part == 2) {
                                break;
                            } else {
                                return result;
                            }
                        }
                    }
                } else {
                    if (part == 2) {
                        break;
                    } else {
                        return result;
                    }
                }
                break;
            }
            map.at(y).at(x) = 'o';
            ++result;
            if (part == 2 && (y == 0 && x == sand_x)) {
                return result;
            }
        }
        return result;
    };

    const auto get_sand_units = [&](const auto& paths, const auto part) {
        const auto [min, max] = get_min_max(paths, part);
        const auto shift = (part == 2) ? (max.y * 2) : 0;
        const auto size = point{max.x - min.x + (shift * 2), max.y};
        const auto sand = point{500 - min.x + shift, 0};
        auto map = create_empty_map(size);
        fill_in_map(map, paths, min, shift);
        return fall_sand(map, sand.x, size, part);
    };

    const auto paths = get_paths(Input);
    const auto result1 = get_sand_units(paths, 1);
    const auto result2 = get_sand_units(paths, 2);
    std::cout << result1 << std::endl;
    std::cout << result2 << std::endl;
    assert(result1 == 832);
    assert(result2 == 27601);
}
