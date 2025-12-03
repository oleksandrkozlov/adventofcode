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

} // namespace

template<std::size_t Len>
consteval auto part_one_two() -> std::size_t
{
    auto to_int = [](char c) { return c - '0'; };
    auto to_num = [](auto&& rng) {
        auto result = 0uz;
        for (auto d : rng) { result = result * 10 + d; };
        return result;
    };
    return rng::fold_left(
        input | rv::split('\n') | rv::filter([](auto&& rng) { return not rng.empty(); }) | rv::transform([&](auto&& l) {
            auto max = std::array<int, Len>{};
            max.fill(-1);
            for (auto i = 0uz, j = 0uz, max_j = 0uz, len = Len; i < Len; ++i, --len, j = max_j + 1) {
                for (; j <= l.size() - len; ++j) {
                    if (auto n = to_int(l[j]); n > max[i]) { max[i] = n, max_j = j; }
                }
            }
            return to_num(max);
        }),
        0uz,
        std::plus{});
}

auto main() -> int
{
    constexpr auto result1 = part_one_two<2>();
    constexpr auto result2 = part_one_two<12>();
    std::println("{}\n{}", result1, result2);
    static_assert(result1 == 17193);
    static_assert(result2 == 171297349921310);
}
