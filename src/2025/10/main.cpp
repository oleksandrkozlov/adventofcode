#include <bits/stdc++.h>
#include <z3++.h>

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

constexpr auto to_num = [](auto sv) {
    auto result = std::size_t{};
    std::from_chars(std::data(sv), std::data(sv) + std::size(sv), result);
    return result;
};

constexpr auto unpair = []<typename F>(F&& f) {
    return [f = std::forward<F>(f)](auto&& pair) {
        auto&& [first, second] = pair;
        return f(first, second);
    };
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

[[nodiscard]] auto part_n(const int part) -> std::size_t
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
        if (part == 1) {
            auto dp = std::unordered_map<std::string, size_t>{};
            auto&& [pattern, combinations, _] = machine;
            return press_buttons(dp, pattern, std::string(pattern.size(), '.'), combinations);
        } else {
            auto ctx = z3::context{};
            auto&& [_, combinations, joltages] = machine;
            const auto vars = combinations
                | rv::enumerate
                | rv::transform([&](auto&& i) { return ctx.int_const(std::format("B{}", i).c_str()); })
                | rng::to<std::vector>();
            auto equals = joltages
                | rv::enumerate
                | rv::transform([&](auto&& js) {
                    auto&& [i, joltage] = js;
                    return ctx.int_val(joltage) == rng::fold_left(combinations | rv::enumerate | rv::filter([&](auto&& comb) {
                        auto&& [_, buttons] = comb;
                        return rng::contains(buttons, i);
                    }) | rv::transform([&](auto&& comb) {
                        auto&& [j, _] = comb;
                        return vars[j];
                    }) | rng::to<std::vector>(), ctx.int_val(0), std::plus{});
                }) | rng::to<std::vector>();
            auto optimizer = z3::optimize{ctx};
            optimizer.minimize(rng::fold_left(vars, ctx.int_val(0), std::plus{}));
            for (auto&& eq : equals) { optimizer.add(eq); }
            for (auto&& var : vars) { optimizer.add(var >= 0); }
            assert(optimizer.check());
            auto model = optimizer.get_model();
            return rng::fold_left(rv::iota(0u, model.size()) |  rv::transform([&](auto&& i){
                return model.get_const_interp(model[i]).get_numeral_int64();
            }), 0uz, std::plus{});
        }
    }), 0, std::plus{});
} // clang-format on

} // namespace

auto main() -> int
{
    const auto result1 = part_n(1);
    const auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 488);
    assert(result2 == 18771);
}
