#include <bits/stdc++.h>

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

constexpr auto to_sv = [](auto rng) { return std::string_view{rng}; };

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

constexpr auto sum = []<typename Rng>(Rng&& rng) { return rng::fold_left(std::forward<Rng>(rng), 0uz, std::plus{}); };

[[nodiscard]] auto solve() -> std::size_t
{ // clang-format off
    auto shapes_and_regions = input | rv::split("\n\n"sv);
    assert(rng::distance(shapes_and_regions) == 7);
    auto shapes_sizes = shapes_and_regions | rv::take(6) | rv::transform([](auto&& rng) { return rng::count(rng, '#'); });
    return sum(shapes_and_regions
        | rv::drop(6)
        | rv::take(1)
        | rv::join
        | rng::to<std::string>()
        | rv::split('\n')
        | rv::filter(std::not_fn(rng::empty))
        | rv::transform(to_sv)
        | rv::transform([&](auto&& sv) {
            auto delim = sv.find(':');
            auto xy = sv.substr(0, delim) | rv::split('x');
            return to_num(xy.front()) * to_num(*std::next(xy.begin())) > sum(sv.substr(delim + 2)
                | rv::split(' ')
                | rv::transform(to_num)
                | rv::enumerate
                | rv::transform(unpair([&](auto&& i, auto&& count) { return *std::next(shapes_sizes.begin(), i) * count; })));
        }));
} // clang-format on

} // namespace

auto main() -> int
{
    const auto result = solve();
    std::println("{}", result);
    assert(result == 476);
}
