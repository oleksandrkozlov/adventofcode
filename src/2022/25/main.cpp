#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

[[nodiscard]] auto parse_input(const std::string_view input) -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    for (const auto row : input | ranges::views::split('\n')) {
        result.push_back(row | ranges::to<std::string>());
    };
    return result;
}

[[nodiscard]] auto to_decimal(const std::string_view snafu) -> std::int64_t
{
    auto result = std::int64_t{};
    auto nums = std::vector<std::int64_t>{};
    for (auto i = static_cast<std::int64_t>(snafu.size() - 1); i >= 0; --i) {
        const auto n = [&] {
            const auto c = snafu.at(i);
            if (c == '-') {
                return std::int64_t{-1};
            } else if (c == '=') {
                return std::int64_t{-2};
            } else {
                return std::stol(std::string{c});
            }
        }();
        nums.push_back(n);
    }
    for (auto i = std::size_t{}; i < std::size(nums); ++i) {
        result += nums.at(i) * static_cast<std::int64_t>(std::pow(5, i));
    }
    return result;
}

[[nodiscard]] auto to_snafu(const std::int64_t decimal) -> std::string
{
    if (decimal == 0) {
        return {};
    }
    const auto str = std::string{"012=-"};
    return to_snafu((decimal + 2) / 5) + str.at(decimal % 5);
}

auto main() -> int
{
    const auto numbers = parse_input(Input);
    auto sum = std::int64_t{};
    for (auto& n : numbers) {
        sum += to_decimal(n);
    }
    const auto result = to_snafu(sum);
    fmt::print("{}\n", result);
    assert(result == "2-==10--=-0101==1201");
}
