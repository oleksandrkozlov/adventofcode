#include <bits/stdc++.h>
#include <range/v3/all.hpp>

#include <algorithm>
#include <iterator>

namespace {

namespace rv = std::views;
namespace rng = std::ranges;

constexpr auto input = std::invoke([] {
    static constexpr char result[] = {
#if 1
#embed "input"
#define AOC_ITERATIONS 1000
#else
#define AOC_ITERATIONS 10
#embed "example"
#endif
    };
    return std::string_view{result, sizeof(result)};
});

using coord_t = std::ptrdiff_t;
using point_t = std::tuple<coord_t, coord_t, coord_t>;

template<typename T>
auto hash_combine(std::size_t& seed, const T& v) -> void
{
    auto hasher = std::hash<T>{};
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

constexpr auto to_num = [](auto sv) {
    auto result = coord_t{};
    std::from_chars(std::data(sv), std::data(sv) + std::size(sv), result);
    return result;
};

constexpr auto to_sv = [](auto rng) { return std::string_view{rng}; };

constexpr auto to_coords
    = [](auto&& p) { return std::tuple{p.front(), *std::next(p.begin()), *std::next(std::next(p.begin()))}; };

constexpr auto square_diff = [](const coord_t lhs, const coord_t rhs) { return std::pow(lhs - rhs, 2); };

constexpr auto unpair = []<typename F>(F&& f) {
    return [f = std::forward<F>(f)](auto&& pair) {
        auto&& [first, second] = pair;
        return f(first, second);
    };
};

constexpr auto find_chain_with = [](auto&& chains, const point_t& p) {
    return rng::find_if(chains, [&](auto&& conn) { return conn.contains(p); });
};

} // namespace

template<>
struct std::hash<point_t> {
    [[nodiscard]] auto operator()(const point_t& p) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto [x, y, z] = p;
        hash_combine(result, std::hash<coord_t>{}(x));
        hash_combine(result, std::hash<coord_t>{}(y));
        hash_combine(result, std::hash<coord_t>{}(z));
        return result;
    }
};

[[nodiscard]] auto part_n(const int part) -> std::size_t
{
    auto coords = input | rv::split('\n') | rv::filter(std::not_fn(rng::empty)) | rv::transform([](auto&& line) {
                      return to_coords(line | rv::split(',') | rv::transform(to_num));
                  });
    const auto distances
        = rv::cartesian_product(coords, coords)
        | rv::filter(unpair(std::not_equal_to{}))
        | rv::transform(unpair([](auto&& p1, auto&& p2) {
              auto&& [x1, y1, z1] = p1;
              auto&& [x2, y2, z2] = p2;
              return std::tuple{square_diff(x2, x1) + square_diff(y2, y1) + square_diff(z2, z1), p1, p2};
          }))
        | rng::to<std::vector>()
        | ranges::actions::sort(std::less{}, [](auto&& p) { return std::get<0>(p); })
        | rv::stride(2);
    auto x1 = coord_t{};
    auto x2 = coord_t{};
    auto chains = std::list<std::unordered_set<point_t>>{};
    for (auto&& [d, p1, p2] : distances | ranges::views::take(part == 1 ? AOC_ITERATIONS : rng::distance(distances))) {
        const auto chain1 = find_chain_with(chains, p1);
        const auto chain2 = find_chain_with(chains, p2);
        const auto has_p1 = chain1 != chains.end();
        const auto has_p2 = chain2 != chains.end();
        if (not has_p1 and not has_p2) {
            chains.push_back({p1, p2});
        } else if (has_p1 and not has_p2) {
            chain1->emplace(p2);
        } else if (has_p2 and not has_p1) {
            chain2->emplace(p1);
        } else {
            assert(has_p1 and has_p2);
            if (*chain1 == *chain2) { continue; }
            chain1->merge(*chain2);
            chains.remove(*chain2);
        }
        x1 = std::get<0>(p1);
        x2 = std::get<0>(p2);
    }
    if (part == 1) {
        return rng::fold_left(
            chains
                | rv::transform(rng::size)
                | rng::to<std::vector>()
                | ranges::actions::sort(std::greater{})
                | rv::take(3),
            1,
            std::multiplies{});
    } else {
        return x1 * x2;
    }
}

auto main() -> int
{
    const auto result1 = part_n(1);
    const auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 135169);
    assert(result2 == 302133440);
}
