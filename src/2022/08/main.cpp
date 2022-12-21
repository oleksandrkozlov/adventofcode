#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <string>
#include <string_view>

[[nodiscard]] auto get_tree_numbers(const std::string_view input) -> int
{
    using namespace std::literals;
    const auto X = 99;
    const auto Y = 99;
    int arr[X][Y];
    int result[X][Y];
    {
        auto i = 0;
        for (const auto range : input | ranges::views::split('\n')) {
            const auto line = range | ranges::to<std::string>();
            int j = 0;
            for (auto c : line) {
                arr[i][j] = std::stoi(std::string{c});
                ++j;
            }
            ++i;
        }
    }
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            result[i][j] = 0;
        }
    }
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if ((i == 0) || (i == X - 1) || (j == 0) || (j == Y - 1)) {
                result[i][j] = 1;
                continue;
            }
            auto is_visible = true;
            for (int x = i + 1; x < X; ++x) {
                if (arr[x][j] >= arr[i][j]) {
                    is_visible = false;
                    break;
                }
            }
            if (is_visible) {
                result[i][j] = 1;
            }
            is_visible = true;
            for (int x = i - 1; x >= 0; --x) {
                if (arr[x][j] >= arr[i][j]) {
                    is_visible = false;
                    break;
                }
            }
            if (is_visible) {
                result[i][j] = 1;
            }
            is_visible = true;
            for (int y = j + 1; y < Y; ++y) {
                if (arr[i][y] >= arr[i][j]) {
                    is_visible = false;
                    break;
                }
            }
            if (is_visible) {
                result[i][j] = 1;
            }
            is_visible = true;
            for (int y = j - 1; y >= 0; --y) {
                if (arr[i][y] >= arr[i][j]) {
                    is_visible = false;
                    break;
                }
            }
            if (is_visible) {
                result[i][j] = 1;
            }
        }
    }
    auto rv = 0;
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if (result[i][j] != 0) {
                ++rv;
            }
        }
    }
    return rv;
}

[[nodiscard]] auto get_highest_scenic_score(const std::string_view input) -> int
{
    const auto X = 99;
    const auto Y = 99;
    int trees[X][Y];
    {
        auto i = 0;
        for (const auto line : input | ranges::views::split('\n')) {
            auto j = 0;
            for (const auto c : line) {
                trees[i][j] = std::stoi(std::string{c});
                ++j;
            }
            ++i;
        }
    }
    auto result = 0;
    for (auto i = 0; i < X; i++) {
        for (auto j = 0; j < Y; j++) {
            auto down = 0;
            for (auto k = i + 1; k < X; ++k) {
                ++down;
                if (trees[k][j] >= trees[i][j]) {
                    break;
                }
            }
            auto up = 0;
            for (auto k = i - 1; k >= 0; --k) {
                ++up;
                if (trees[k][j] >= trees[i][j]) {
                    break;
                }
            }
            auto right = 0;
            for (auto k = j + 1; k < Y; ++k) {
                ++right;
                if (trees[i][k] >= trees[i][j]) {
                    break;
                }
            }
            auto left = 0;
            for (auto k = j - 1; k >= 0; --k) {
                ++left;
                if (trees[i][k] >= trees[i][j]) {
                    break;
                }
            }
            result = std::max(result, down * up * right * left);
        }
    }
    return result;
}

int main()
{
    const auto result0 = get_tree_numbers(Input);
    const auto result1 = get_highest_scenic_score(Input);
    fmt::print("{}\n{}\n", result0, result1);
    assert(result0 == 1814);
    assert(result1 == 330786);
}
