#include <bits/stdc++.h>

namespace {

namespace rv = std::views;
namespace rng = std::ranges;

constexpr auto input = std::invoke([] {
    static constexpr char result[] = {
#if 1
#embed "input"
#else
#embed "example"
#endif
    };
    return std::string_view{result, sizeof(result)};
});

constexpr auto unpair = []<typename F>(F&& f) {
    return [f = std::forward<F>(f)](auto&& pair) {
        auto&& [first, second] = pair;
        return f(first, second);
    };
};

constexpr auto dir
    = std::array<std::pair<int, int>, 8>{{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

auto to_grid()
{
    constexpr auto Len = rng::count(input, '\n');
    auto grid = std::array<std::array<char, Len>, Len>{};
    for (auto&& [j, row] : input
             | rv::split('\n') //
             | rv::filter([](auto&& rng) { return not rng.empty(); })
             | rv::enumerate) {
        for (auto&& [i, value] : row | rv::enumerate) {
            assert(i <= Len - 1);
            assert(j <= Len - 1);
            grid[j][i] = value;
        }
    }
    return grid;
}

} // namespace

auto part_one_two() -> std::pair<std::size_t, std::size_t>
{
    auto result1 = 0uz;
    auto result2 = 0uz;
    auto grid = to_grid();
    auto grid_copy = grid;
    auto count = -1;
    auto first = true;

    while (count != 0) {
        count = 0;
        grid = grid_copy;
        for (auto [i, row] : grid | rv::enumerate) {
            for (auto [j, value] : row | rv::enumerate) {
                if (value != '@') { continue; } // clang-format off
                if (4 > rng::count_if(dir, unpair([&](const int di, const int dj) {
                    const auto x = i + di;
                    const auto y = j + dj;
                    return x >= 0 and x < std::ssize(row) and y >= 0 and y < std::ssize(grid) and grid[x][y] == '@';
                }))) { // clang-format on
                    grid_copy[i][j] = '.';
                    ++count;
                }
            }
        }
        result2 += count;
        if (first) {
            result1 = result2;
            first = false;
        }
    }
    return {result1, result2};
}

auto main() -> int
{
    const auto result = part_one_two();
    std::println("{}\n{}", result.first, result.second);
    assert(result.first == 1505);
    assert(result.second == 9182);
}
