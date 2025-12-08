#include <bits/stdc++.h>
#include <range/v3/all.hpp>

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

constexpr auto to_distance = unpair([](auto&& p1, auto&& p2) {
    auto&& [x1, y1, z1] = p1;
    auto&& [x2, y2, z2] = p2;
    return std::tuple{square_diff(x2, x1) + square_diff(y2, y1) + square_diff(z2, z1), p1, p2};
});

constexpr auto by_not_equal_to = unpair(std::not_equal_to{});

constexpr auto by_distance = [](auto&& p) { return std::get<0>(p); };

constexpr auto split_and_transform_to_coords = [] {
    return rv::split('\n')
        | rv::filter(std::not_fn(rng::empty))
        | rv::transform([](auto&& line) { return to_coords(line | rv::split(',') | rv::transform(to_num)); });
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

template<typename Key>
struct dsu {
    auto add(const Key& x) -> void
    {
        if (parent.contains(x)) { return; }
        parent[x] = x;
        sz[x] = 1;
        ++components;
    }

    auto add_range(auto&& xs) -> void
    {
        for (auto&& x : xs) { add(x); }
    }

    [[nodiscard]] auto find(const Key& x) const -> const Key&
    {
        if (parent[x] != x) { parent[x] = find(parent[x]); };
        return parent[x];
    }

    auto unite(const Key& a, const Key& b) -> void
    {
        auto ra = find(a);
        auto rb = find(b);
        if (ra == rb) { return; };
        if (sz[ra] < sz[rb]) { std::swap(ra, rb); }
        parent[rb] = ra;
        sz[ra] += sz[rb];
        --components;
    }

    [[nodiscard]] auto group_size(const Key& x) const -> std::size_t
    {
        return sz[find(x)];
    }

    [[nodiscard]] auto all_group_sizes() const -> std::vector<std::size_t>
    {
        return parent
            | rv::filter([&](auto&& kv) { return find(kv.first) == kv.first; })
            | rv::transform([&](auto&& kv) { return sz[kv.first]; })
            | rng::to<std::vector>();
    }

    mutable std::unordered_map<Key, Key> parent;
    mutable std::unordered_map<Key, std::size_t> sz;
    std::size_t components{};
};

[[nodiscard]] auto part_n(const int part) -> std::size_t
{
    auto coords = input | split_and_transform_to_coords();
    const auto coords_size = rng::distance(coords);
    const auto iterations = (coords_size * (coords_size - 1)) * 2;
    const auto distances = rv::cartesian_product(coords, coords)
        | rv::filter(by_not_equal_to)
        | rv::transform(to_distance)
        | rng::to<std::vector>()
        | ranges::actions::sort(std::less{}, by_distance)
        | rv::stride(2)
        | rv::take(part == 1 ? AOC_ITERATIONS : iterations);

    auto uf = dsu<point_t>{};
    uf.add_range(coords);
    for (auto&& [_, p1, p2] : distances) {
        uf.unite(uf.find(p1), uf.find(p2));
        if (part == 2 and uf.components == 1) { return std::get<0>(p1) * std::get<0>(p2); }
    }
    return rng::fold_left(
        uf.all_group_sizes() | ranges::actions::sort(std::greater{}) | rv::take(3), 1, std::multiplies{});
}

auto main() -> int
{
    const auto result1 = part_n(1);
    const auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 135169);
    assert(result2 == 302133440);
}
