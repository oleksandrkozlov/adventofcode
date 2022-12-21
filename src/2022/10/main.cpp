#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <string_view>

[[nodiscard]] auto get_signal_strengths(const std::string_view input) -> std::pair<int, std::string>
{
    using namespace std::literals;
    auto cycle = 0;
    auto X = 1;
    auto result = 0;
    auto sprite = std::string(40, ' ');
    auto set_sprite = [&](auto pos) {
        sprite = std::string(40, ' ');
        if (((pos - 1) >= 0) && (pos - 1) < 40) {
            sprite.at(pos - 1) = '#';
        }
        if ((pos >= 0) && pos + 1 < 40) {
            sprite.at(pos) = '#';
        }
        if (((pos + 1) >= 0) && (pos + 1) < 40) {
            sprite.at(pos + 1) = '#';
        }
    };
    auto row1 = std::string(40, ' ');
    auto row2 = std::string(40, ' ');
    auto row3 = std::string(40, ' ');
    auto row4 = std::string(40, ' ');
    auto row5 = std::string(40, ' ');
    auto row6 = std::string(40, ' ');
    auto pos = -1;
    set_sprite(1);
    const auto update = [&]() {
        if (cycle >= 1 && cycle <= 40) {
            if (sprite.at(pos) == '#') {
                row1.at(pos) = '#';
            }
            if (cycle == 20) {
                result += cycle * X;
            }
        }
        if (cycle >= 41 && cycle <= 80) {
            if (sprite.at(pos) == '#') {
                row2.at(pos) = '#';
            }
            if (cycle == 60) {
                result += cycle * X;
            }
        }
        if (cycle >= 81 && cycle <= 120) {
            if (sprite.at(pos) == '#') {
                row3.at(pos) = '#';
            }
            if (cycle == 100) {
                result += cycle * X;
            }
        }
        if (cycle >= 121 && cycle <= 160) {
            if (sprite.at(pos) == '#') {
                row4.at(pos) = '#';
            }
            if (cycle == 140) {
                result += cycle * X;
            }
        }
        if (cycle >= 161 && cycle <= 200) {
            if (sprite.at(pos) == '#') {
                row5.at(pos) = '#';
            }
            if (cycle == 180) {
                result += cycle * X;
            }
        }
        if (cycle >= 201 && cycle <= 240) {
            if (sprite.at(pos) == '#') {
                row6.at(pos) = '#';
            }
            if (cycle == 220) {
                result += cycle * X;
            }
        }
    };
    for (const auto range : input | ranges::views::split('\n')) {
        auto signal = range | ranges::to<std::string>();
        if (pos == 40) {
            pos = 0;
        }
        if (signal == "noop") {
            ++pos;
            if (pos == 40) {
                pos = 0;
            }
            ++cycle;
            update();
        } else {
            for (auto i = 0; i < 2; ++i) {
                ++pos;
                if (pos == 40) {
                    pos = 0;
                }
                ++cycle;
                update();
                if (i == 1) {
                    X += std::stoi(signal.substr(std::size("addx")));
                    set_sprite(X);
                }
            }
        }
    }
    const auto output = fmt::format("{}\n{}\n{}\n{}\n{}\n{}", row1, row2, row3, row4, row5, row6);
    return {result, output};
}

auto main() -> int
{
    const auto [result0, result1] = get_signal_strengths(Input);
    fmt::print("{}\n{}\n", result0, result1);
    const auto output = R"(###  ###    ##  ##  ####  ##   ##  ###  
#  # #  #    # #  #    # #  # #  # #  # 
###  #  #    # #  #   #  #    #  # #  # 
#  # ###     # ####  #   # ## #### ###  
#  # #    #  # #  # #    #  # #  # #    
###  #     ##  #  # ####  ### #  # #    )";
    assert(result0 == 15140);
    assert(result1 == output);
}
