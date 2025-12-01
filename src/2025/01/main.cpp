#include <bits/stdc++.h>

namespace {

namespace rv = std::views;

constexpr auto input = std::invoke([] -> std::string_view {
    static constexpr char result[] = {
#embed "input"
    };
    return {result, sizeof(result)};
});

consteval auto part_n(int part) -> int
{
    auto result = 0;
    auto d = 50;
    auto lines = input
        | rv::split('\n')
        | rv::filter([](auto rng) { return rng.size() != 0; })
        | rv::transform([](auto rng) -> std::string_view { return {rng.data(), rng.size()}; });

    for (auto&& l : lines) {
        auto n = int{};
        std::from_chars(l.data() + 1, l.data() + l.size(), n);
        auto c = l.at(0);
        if (part == 1) {
            d = (c == 'R' ? d + n : d - n) % 100;
            if (d < 0) { d += 100; }
            if (d == 0) { ++result; }
        } else {
            for (auto i = 0; i < n; ++i) {
                d = c == 'R' ? d + 1 : d - 1;
                d = d == 100 ? 0 : d == -1 ? 99 : d;
                if (d == 0) { ++result; }
            }
        }
    }
    return result;
}

} // namespace

auto main() -> int
{
    static constexpr auto result1 = part_n(1);
    static constexpr auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    static_assert(result1 == 1031);
    static_assert(result2 == 5831);
}
