#include <bits/stdc++.h>

#include <algorithm>
#include <ranges>

namespace {

namespace rv = std::views;
namespace rng = std::ranges;
using namespace std::literals;

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

} // namespace

auto part_n(int part) -> std::size_t
{
    static constexpr auto back = [](auto&& rng) -> decltype(auto) { return *std::next(rng.begin()); };
    static constexpr auto to_pair = [&](auto&& rng) { return std::pair{rng.front(), back(rng)}; };
    static constexpr auto to_num = [](auto&& rng) {
        auto result = 0uz;
        std::from_chars(rng.data(), rng.data() + rng.size(), result);
        return result;
    };
    auto lists = input | rv::split("\n\n"sv);
    if (part == 1) {
        auto result = 0uz;
        rng::for_each(back(lists) | rv::split('\n') | rv::transform(to_num), [&](const std::size_t id) {
            result += rng::any_of(lists.front() | rv::split('\n'), [&](auto&& range) {
                auto&& [start, end] = to_pair(range | rv::split('-'));
                return id >= to_num(start) and id <= to_num(end);
            });
        });
        return result;
    } else {
        auto pairs = lists.front()
            | rv::split('\n')
            | rv::transform([&](auto&& range) { // clang-format off
                auto&& [start, end] = to_pair(range | rv::split('-'));
                return std::pair{to_num(start), to_num(end)};
              })
            | rng::to<std::vector>();
        rng::sort(pairs);
        auto cur = 0uz;
        return rng::fold_left(pairs | rv::transform(unpair([&](const std::size_t start, const std::size_t end) {
            if (cur < start) {
                cur = end;
                return end - start + 1;
            }
            if (cur < end) { return end - std::exchange(cur, end); }
            return 0uz;
        })), 0uz, std::plus{}); // clang-format on
    }
}

auto main() -> int
{

    const auto result1 = part_n(1);
    const auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 811);
    assert(result2 == 338189277144473);
}
