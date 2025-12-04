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

auto part_n(int part) -> std::size_t
{
    constexpr auto to_sv = [](auto rng) { return std::string_view{rng}; };
    constexpr auto to_num = [](auto sv) {
        auto result = 0uz;
        std::from_chars(sv.data(), sv.data() + sv.size(), result);
        return result;
    };
    auto set = std::unordered_set<std::size_t>();
    for (auto&& range : input | rv::split(',')) {
        auto pair = range | rv::split('-');
        auto start = to_num(pair.front());
        auto end = to_num(*std::next(pair.begin())) + 1;
        for (auto i : rv::iota(start, end)) {
            auto s = std::to_string(i);
            if (part == 1) {
                if (s.size() % 2 != 0) { continue; }
                auto half = s | rv::chunk(static_cast<float>(s.size()) * 0.5f) | rv::transform(to_sv);
                if (half.front() == half.back()) { set.insert(i); }
            } else { // clang-format off
                for (auto _ : rv::iota(1uz, s.size()) | rv::filter([&](std::size_t j) {
                    return 1 == (s | rv::chunk(j) | rv::transform(to_sv) | rng::to<std::unordered_set>()).size();
                 })) { set.insert(i); } // clang-format on
            }
        }
    }
    return rng::fold_left(set, 0uz, std::plus{});
}

} // namespace

auto main() -> int
{
    auto result1 = part_n(1);
    auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 53420042388);
    assert(result2 == 69553832684);
}
