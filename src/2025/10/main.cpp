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
constexpr auto to_sv = [](auto rng) { return std::string_view{rng}; };

constexpr auto to_num = [](auto sv) {
    auto result = std::size_t{};
    std::from_chars(std::data(sv), std::data(sv) + std::size(sv), result);
    return result;
};

auto press_buttons(
    std::unordered_map<std::string, std::size_t>& dp,
    const std::string& goal,
    const std::string& curent,
    const std::vector<std::vector<std::size_t>>& buttons) -> std::size_t
{
    if (curent == goal) { return 0; };
    if (dp.contains(curent)) { return dp[curent]; };
    dp[curent] = SIZE_MAX;
    auto best = SIZE_MAX;
    for (auto&& button : buttons) {
        auto next = curent;
        for (auto i : button) {
            auto& c = next[i];
            c = c == '.' ? '#' : '.';
        }
        auto cost = press_buttons(dp, goal, next, buttons);
        if (cost != SIZE_MAX) { best = std::min(best, 1 + cost); }
    }
    dp[curent] = best;
    return best;
}

auto part_one() -> std::size_t
{ // clang-format off
    auto machines = input
        | rv::split('\n')
        | rv::filter(std::not_fn(rng::empty))
        | rv::transform([](auto&& rng) {
            auto diagram_buttons_joltage = rng
                | rv::split(' ')
                | rv::chunk_by([](auto&& lhs, auto rhs) {
                    return not(lhs.front() == '[' or rhs.front() == '{');
                });
            auto diagram_with_brakets = diagram_buttons_joltage.front() | rv::join;
            auto diagram = diagram_with_brakets
                | rv::drop(1)
                | rv::take(rng::distance(diagram_with_brakets) - 2)
                | rng::to<std::string>();
            auto buttons = *std::next(diagram_buttons_joltage.begin())
                | rv::transform([](auto&& nums_with_parentheses) {
                    return nums_with_parentheses
                        | rv::drop(1)
                        | rv::take(std::size(nums_with_parentheses) - 2)
                        | rv::split(',')
                        | rv::transform(to_num)
                        | rng::to<std::vector>();
                })
                | rng::to<std::vector>();
            auto joltage_with_brakets = *std::next(std::next(diagram_buttons_joltage.begin())) | rv::join;
            auto joltage = joltage_with_brakets
                | rv::drop(1)
                | rv::take(rng::distance(joltage_with_brakets) - 2)
                | rng::to<std::string>()
                | rv::split(',')
                | rv::transform(to_num)
                | rng::to<std::vector>();
            return std::tuple{diagram, buttons, joltage};
    });
    return rng::fold_left(machines | rv::transform([&](auto&& machine) {
        auto dp = std::unordered_map<std::string, size_t>{};
        auto&& [pattern, combinations, _] = machine;
        return press_buttons(dp, pattern, std::string(pattern.size(), '.'), combinations);
    }), 0, std::plus{});
} // clang-format on

} // namespace

auto main() -> int
{
    const auto result1 = part_one();
    std::println("{}", result1);
    assert(result1 == 488);
}
