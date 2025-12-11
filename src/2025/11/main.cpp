#include <bits/stdc++.h>

template<typename T>
auto hash_combine(std::size_t& seed, const T& v) -> void
{
    auto hasher = std::hash<T>{};
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<>
struct std::hash<std::tuple<std::string, bool, bool>> {
    [[nodiscard]] auto operator()(const std::tuple<std::string, bool, bool>& value) const -> std::size_t
    {
        auto result = std::size_t{};
        auto&& [node, b1, b2] = value;
        hash_combine(result, std::hash<std::string>{}(node));
        hash_combine(result, std::hash<bool>{}(b1));
        hash_combine(result, std::hash<bool>{}(b2));
        return result;
    }
};

namespace {

namespace rv = std::views;
namespace rng = std::ranges;
using namespace std::literals;

constexpr auto input = std::invoke([] {
    static constexpr char result[] = {
#if 1
#embed "input"
#else
// #embed "example_one"
#embed "example_two"
#endif
    };
    return std::string_view{result, sizeof(result)};
});

template<typename Value>
[[nodiscard]] auto not_equal_to(Value&& value)
{
    return std::bind_front(std::not_equal_to{}, std::forward<Value>(value));
}

using node_t = std::string;
using input_t = std::tuple<node_t, bool, bool>;

auto solve_part_two(
    std::unordered_map<input_t, std::size_t>& dp,
    const std::map<std::string, std::vector<node_t>>& devices,
    const input_t& inputs) -> std::size_t
{ // clang-format off
    auto&& [node, seen_dac, seen_fft] = inputs;
    if (dp.contains(inputs)) { return dp.at(inputs); }
    if (node == "out") {
        return dp.emplace(inputs, seen_dac and seen_fft ? 1uz : 0uz).first->second;
    } else {
        return dp.emplace(inputs, rng::fold_left(devices.at(node) | rv::transform([&](auto&& child) {
            return solve_part_two(dp, devices, {child, seen_dac or child == "dac", seen_fft or child == "fft"});
        }), 0uz, std::plus{})) .first->second;
    }
} // clang-format on

auto part_n(const int part) -> std::size_t
{ // clang-format off
    const auto devices = input
        | rv::split('\n')
        | rv::filter(std::not_fn(rng::empty))
        | rv::transform([](auto&& rng) {
            auto nodes = rng | rv::split(' ');
            return std::pair{
                nodes | rv::take(1) | rv::join | rv::take_while(not_equal_to(':')) | rng::to<node_t>(),
                nodes | rv::drop(1) | rng::to<std::vector<node_t>>()};
        })
        | rng::to<std::map>();
    if (part == 1) {
        auto s = std::stack<node_t>{{"you"}};
        auto result = std::size_t{};
        while (!s.empty()) {
            auto node = s.top();
            s.pop();
            if (node == "out") {
                ++result;
                continue;
            }
            for (auto&& child : devices.at(node)) { s.push(child); }
        }
        return result;

    } else {
        auto dp = std::unordered_map<input_t, std::size_t>{};
        return solve_part_two(dp, devices, {"svr", false, false});
    }
}

} // namespace

auto main() -> int
{
    const auto result1 = part_n(1);
    const auto result2 = part_n(2);
    std::println("{}\n{}", result1, result2);
    assert(result1 == 508);
    assert(result2 == 315116216513280);
}
