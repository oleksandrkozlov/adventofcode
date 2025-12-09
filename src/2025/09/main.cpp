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

using coord_t = std::ptrdiff_t;
using point_t = std::pair<coord_t, coord_t>;
using line_t = std::pair<point_t, point_t>;

constexpr auto to_coords = [](auto&& p) { return std::pair{p.front(), *std::next(p.begin())}; };

constexpr auto to_num = [](auto sv) {
    auto result = coord_t{};
    std::from_chars(std::data(sv), std::data(sv) + std::size(sv), result);
    return result;
};
constexpr auto unpair = []<typename F>(F&& f) {
    return [f = std::forward<F>(f)](auto&& pair) {
        auto&& [first, second] = pair;
        return f(first, second);
    };
};

constexpr auto by_not_equal_to = unpair(std::not_equal_to{});

constexpr auto to_area = unpair([](auto&& p1, auto&& p2) {
    auto&& [x1, y1] = p1;
    auto&& [x2, y2] = p2;
    return (std::abs(x2 - x1) + 1) * (std::abs(y2 - y1) + 1);
});

constexpr auto split_and_transform_to_coords = [] {
    return rv::split('\n')
        | rv::filter(std::not_fn(rng::empty))
        | rv::transform([](auto&& line) { return to_coords(line | rv::split(',') | rv::transform(to_num)); });
};

[[nodiscard]] auto is_intersect(coord_t a1, coord_t a2, coord_t b1, coord_t b2) -> bool
{
    if (a1 > a2) { std::swap(a1, a2); }
    if (b1 > b2) { std::swap(b1, b2); }
    return std::max(a1, b1) < std::min(a2, b2);
}

[[nodiscard]] auto is_intersect(const point_t& s1, const point_t& s2, const point_t& p1, const point_t& p2) -> bool
{
    auto&& [px1, py1] = p1;
    auto&& [px2, py2] = p2;
    auto&& [sx1, sy1] = s1;
    auto&& [sx2, sy2] = s2;
    const auto xmin = std::min(px1, px2);
    const auto xmax = std::max(px1, px2);
    const auto ymin = std::min(py1, py2);
    const auto ymax = std::max(py1, py2);
    if (sx1 == sx2) { return (xmin < sx1 and sx1 < xmax) and is_intersect(sy1, sy2, ymin, ymax); }
    assert(sy1 == sy2);
    return (ymin < sy1 and sy1 < ymax) and is_intersect(sx1, sx2, xmin, xmax);
}

[[nodiscard]] auto part_one() -> std::size_t
{
    auto coords = input | split_and_transform_to_coords();
    return rng::max(rv::cartesian_product(coords, coords) | rv::transform(to_area));
}

[[nodiscard]] auto part_two() -> std::size_t
{ // clang-format off
    auto coords = input | split_and_transform_to_coords();
    auto cart_prod = rv::cartesian_product(coords, coords);
    auto dup_lines = cart_prod | rv::filter(unpair([](const point_t& p1, const point_t& p2) {
        auto&& [x1, y1] = p1;
        auto&& [x2, y2] = p2;
        return (p1 != p2) and ((x1 == x2) or (y1 == y2));
    }));
    auto lines = std::vector<line_t>{};
    for (auto&& [p1, p2] : dup_lines) {
        if (rng::contains(lines, std::pair{p2, p1})) { continue; }
        lines.emplace_back(p1, p2);
    }
    return rng::max(cart_prod
        | rv::filter(by_not_equal_to)
        | rv::filter(unpair([&](const point_t& p1, const point_t& p2) {
              return not rng::any_of(lines, unpair([&](const point_t& s1, const point_t& s2) { return is_intersect(s1, s2, p1, p2); }));
          }))
        | rv::transform(to_area));
} // clang-format on

} // namespace

auto main() -> int
{
    const auto result1 = part_one();
    const auto result2 = part_two();
    std::println("{}\n{}", result1, result2);
    assert(result1 == 4758598740);
    assert(result2 == 1474699155);
}
