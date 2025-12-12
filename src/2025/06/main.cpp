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

template<rng::input_range Rngs>
    requires rng::input_range<rng::range_reference_t<Rngs>>
class interleave_view : public rng::view_interface<interleave_view<Rngs>> {
public:
    struct iterator;

    interleave_view() = default;

    explicit interleave_view(Rngs rs)
        : rngs{std::move(rs) | rng::to<std::vector>()}
    {
    }

    [[nodiscard]] auto begin() -> iterator
    {
        return {0, &rngs, rv::transform(rngs, rng::begin) | rng::to<std::vector>()};
    }

    [[nodiscard]] auto end() const -> std::default_sentinel_t
    {
        return std::default_sentinel;
    }

    std::vector<rng::range_value_t<Rngs>> rngs;
};

template<rng::input_range Rngs>
    requires rng::input_range<rng::range_reference_t<Rngs>>
struct interleave_view<Rngs>::iterator {
    using value_type = rng::range_value_t<rng::range_reference_t<Rngs>>;
    using difference_type = std::ptrdiff_t;

    [[nodiscard]] auto operator*() const -> value_type
    {
        return *its[n];
    }

    auto operator++() -> iterator&
    {
        if (0 == ((++n) %= its.size())) {
            rng::for_each(its, [](auto& it) { ++it; });
        }
        return *this;
    }

    auto operator++(int) -> void
    {
        ++*this;
    }

    [[nodiscard]] friend auto operator==(const iterator& it, std::default_sentinel_t) -> bool
    {
        if (it.n != 0) { return false; }
        auto ends = *it.rngs | rv::transform(rng::end);
        return it.its.end() != std::mismatch(it.its.begin(), it.its.end(), ends.begin(), std::not_equal_to<>{}).first;
    }

    [[nodiscard]] auto operator==(const iterator& that) const -> bool
    {
        return n == that.n && its == that.its;
    }

    std::size_t n{};
    std::vector<rng::range_value_t<Rngs>>* rngs;
    std::vector<rng::iterator_t<rng::range_value_t<Rngs>>> its;
};

struct interleave_view_t : rng::range_adaptor_closure<interleave_view_t> {

    auto operator()(auto&& rngs) const
    {
        return interleave_view{std::move(rngs)};
    }
};

constexpr auto interleave = interleave_view_t{};

struct transpose_view_t : rng::range_adaptor_closure<transpose_view_t> {

    auto operator()(auto&& rngs) const
    {
        return rngs | interleave | rv::chunk(rng::distance(rngs));
    }
};

constexpr auto transpose = transpose_view_t{};

constexpr auto to_sv = [](auto rng) { return std::string_view{rng}; };
constexpr auto to_num = [](auto sv) {
    auto result = 0uz;
    std::from_chars(sv.data(), sv.data() + sv.size(), result);
    return result;
};

auto part_one() -> std::size_t
{ // clang-format off
    auto lines = input | rv::split('\n') | rv::filter(std::not_fn(rng::empty)) | rv::transform([](auto&& line) {
        return line | rv::split(' ') | rv::filter(std::not_fn(rng::empty)) | rv::transform(to_sv);
    });
    return rng::fold_left(lines | transpose | rv::transform([](auto&& line) {
        auto not_sign = [](auto&& rng) { return rng != "+" and rng != "*"; };
        auto nums = line | rv::take_while(not_sign) | rv::transform(to_num) | rng::to<std::vector>();
        return *(line | rv::drop_while(not_sign)).begin() == "*" ? rng::fold_left(nums, 1uz, std::multiplies{})
                                                                 : rng::fold_left(nums, 0uz, std::plus{});
    }), 0uz, std::plus{}); // clang-format on
}

auto part_two() -> std::size_t
{
    auto is_space = [](unsigned char c) { return std::isspace(c); };
    auto is_blank = [&](const std::string_view sv) { return rng::all_of(sv, is_space); };
    auto to_str = [](auto&& rng) { return rng | rng::to<std::string>(); };
    auto lines = input | rv::split('\n') | rv::filter(std::not_fn(rng::empty));
    auto chunks = lines // clang-format off
        | transpose
        | rng::to<std::vector<std::string>>()
        | rv::chunk_by([&](auto&& rhs, auto&& lhs) { return !is_blank(to_str(rhs)) and !is_blank(to_str(lhs)); })
        | rv::filter([&](auto&& chunk) { return !is_blank(to_str(chunk.front())); }) // clang-format on
        | rng::to<std::vector<std::deque<std::string>>>();
    for (auto&& chunk : chunks) {
        for (auto&& str : chunk) {
            if (auto i = str.find_first_of('*'); i != std::string::npos) {
                str[i] = ' ';
                chunk.push_front("*");
            } else if (auto j = str.find_first_of('+'); j != std::string::npos) {
                str[j] = ' ';
                chunk.push_front("+");
            }
            str = to_str(str | rv::drop_while(is_space) | rv::take_while(std::not_fn(is_space)));
        }
    } // clang-format off
    return rng::fold_left(chunks | rv::transform([](auto&& chunk) {
        auto nums = chunk | rv::drop(1) | rv::transform(to_num);
        return chunk[0] == "*" ? rng::fold_left(nums, 1uz, std::multiplies{})
                               : rng::fold_left(nums, 0uz, std::plus{});
    }), 0uz, std::plus{}); // clang-format on
}

} // namespace

auto main() -> int
{
    const auto result1 = part_one();
    const auto result2 = part_two();
    std::println("{}\n{}", result1, result2);
    assert(result1 == 4405895212738);
    assert(result2 == 7450962489289);
}
