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

struct point {
    std::ptrdiff_t y{};
    std::ptrdiff_t x{};
};

[[nodiscard]] auto part_one(
    std::vector<std::string> grid, const std::ptrdiff_t W, const std::ptrdiff_t H, const point& start) -> std::size_t
{
    auto result = 0uz;
    auto q = std::queue<point>{};
    q.push(start);
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        if (const auto y = p.y + 1; y < H) {
            if (char& c = grid.at(y).at(p.x); c == '.') {
                c = '|';
                q.push({y, p.x});
            } else if (c == '^') {
                c = '#';
                if (p.x - 1 >= 0) { q.push({y, p.x - 1}); }
                if (p.x + 1 < W) { q.push({y, p.x + 1}); };
                ++result;
            }
        }
    }
    return result;
}

[[nodiscard]] auto part_two(
    const std::vector<std::string>& grid,
    const std::ptrdiff_t W,
    const std::ptrdiff_t H,
    const point& p,
    std::vector<std::vector<std::size_t>>& dp) -> std::size_t
{
    const auto [y, x] = p;
    if (y >= H or y < 0) { return 1; };
    if (x >= W or x < 0) { return 1; };
    auto& res = dp[y][x];
    if (res != static_cast<std::size_t>(-1)) { return res; };

    if (const auto cell = grid[y][x]; cell == 'S' or cell == '.') {
        res = part_two(grid, W, H, {y + 1, x}, dp);
    } else if (cell == '^') {
        res = part_two(grid, W, H, {y + 1, x - 1}, dp) + part_two(grid, W, H, {y + 1, x + 1}, dp);
    } else {
        std::unreachable();
    }
    return res;
}

} // namespace

auto main() -> int
{
    const auto grid
        = input | rv::split('\n') | rv::filter(std::not_fn(rng::empty)) | rng::to<std::vector<std::string>>();
    const auto start = point{.y = 0, .x = static_cast<std::ptrdiff_t>(grid[0].find('S'))};
    const auto H = std::ssize(grid);
    const auto W = std::ssize(grid[0]);
    auto dp = std::vector<std::vector<std::size_t>>(H, std::vector<std::size_t>(W, -1));
    const auto result1 = part_one(grid, W, H, start);
    const auto result2 = part_two(grid, W, H, start, dp);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 1602);
    assert(result2 == 135656430050438);
}
