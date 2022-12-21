#include "input.h"

#include <fmt/color.h>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <iterator>
#include <string>
#include <string_view>

[[nodiscard]] auto is_number(const std::string_view s) -> bool
{
    // clang-format off
    return !(s.empty()
        || (s.find(',') != std::string_view::npos)
        || (s.find('[') != std::string_view::npos)
        || (s.find(']') != std::string_view::npos));
    // clang-format on
}

[[nodiscard]] auto parse(const std::string& str) -> std::vector<std::string>
{
    auto result = std::vector<std::string>{};
    auto items = str.substr(1, str.size() - 2);
    if (items.empty()) {
        return result;
    }
    if ((items.find("[") == std::string::npos) && (items.find(",") == std::string::npos)) {
        result.push_back(items);
        return result;
    }
    if ((items.find("[") == std::string::npos) && (items.find(",") != std::string::npos)) {
        for (auto number : items | ranges::views::split(',')) {
            result.push_back(number | ranges::to<std::string>());
        }
        return result;
    }
    while (!std::empty(items)) {
        if (!items.starts_with("[")) {
            if (const auto pos = items.find(','); pos != std::string::npos) {
                result.push_back(items.substr(0, pos));
                items = items.substr(pos);
                if (items.starts_with(",")) {
                    items = items.substr(1);
                }
            } else {
                result.push_back(items);
                items.clear();
            }
        } else {
            if (items.starts_with("[")) {
                auto new_item = std::string{};
                auto open_bracket_n = 0;
                auto close_bracket_n = 0;
                for (const auto c : items) {
                    new_item += c;
                    if (c == '[') {
                        ++open_bracket_n;
                    }
                    if (c == ']') {
                        ++close_bracket_n;
                    }
                    if (open_bracket_n == close_bracket_n) {
                        result.push_back(new_item);
                        items = items.substr(std::size(new_item));
                        if (items.starts_with(",")) {
                            items = items.substr(1);
                        }
                        break;
                    }
                }
            }
        }
    }
    return result;
}

[[nodiscard]] auto compare(
    const std::string& unparsed_left,
    const std::string& unparsed_right,
    bool& found) -> bool
{
    // fmt::print("{} vs {}\n", unparsed_left, unparsed_right);
    auto result = false;
    [[maybe_unused]] const auto correct = fmt::fg(fmt::color::light_green);
    [[maybe_unused]] const auto wrong = fmt::fg(fmt::color::light_salmon);
    auto parsed_left = parse(unparsed_left);
    auto parsed_right = parse(unparsed_right);
    const auto size_left = std::size(parsed_left);
    const auto size_right = std::size(parsed_right);
    const auto size_min = std::min(size_left, size_right);
    for (auto i = std::size_t{}; i < size_min; ++i) {
        auto& left_str = parsed_left.at(i);
        auto& right_str = parsed_right.at(i);
        if (const auto is_left_numer = is_number(left_str), is_right_numer = is_number(right_str);
            is_left_numer && is_right_numer) {
            const auto left = std::stoi(left_str);
            const auto right = std::stoi(right_str);
            // fmt::print("{} vs {}\n", left, right);
            if (left == right) {
                continue;
            }
            if (left < right) {
                // fmt::print(correct, "Left side is smaller\n");
                found = true;
                result = true;
                break;
            }
            assert(left > right);
            {
                // fmt::print(wrong, "Right side is smaller\n");
                found = true;
                result = false;
                break;
            } // clang-format off
        } else if (const auto is_only_right_number = !is_left_numer && is_right_numer,
                              is_only_left_number = is_left_numer && !is_right_numer; // clang-format on
                   is_only_right_number || is_only_left_number) {
            // fmt::print("{} vs {}\n", left_str, right_str);
            if (is_only_right_number) {
                right_str = fmt::format("[{}]", right_str);
                // fmt::print("Convert right to {}\n", right_str);
                const auto rv = compare(left_str, right_str, found);
                if (found) {
                    result = rv;
                    break;
                }
            } else if (is_only_left_number) {
                left_str = fmt::format("[{}]", left_str);
                // fmt::print("Convert left to {}\n", left_str);
                const auto rv = compare(left_str, right_str, found);
                if (found) {
                    result = rv;
                    break;
                }
            }
        }
        const auto rv = compare(left_str, right_str, found);
        if (found) {
            result = rv;
            break;
        }
    }
    if (!found) {
        if (size_left < size_right) {
            result = true;
            found = true;
            // fmt::print(correct, "Left side ran out of items\n");
        } else if (size_left > size_right) {
            result = false;
            found = true;
            // fmt::print(wrong, "Right side ran out of items\n");
        }
    }
    return result;
}

int main()
{
    auto pairs = std::vector<std::pair<std::string, std::string>>{};
    for (const auto range : Input | ranges::views::split(ranges::views::c_str("\n\n"))) {
        auto pair = range | ranges::views::split('\n');
        const auto left = *ranges::begin(pair) | ranges::to<std::string>();
        const auto right = *ranges::next(ranges::begin(pair)) | ranges::to<std::string>();
        pairs.emplace_back(left, right);
    }
    {
        auto pair_n = 1;
        auto sum = 0;
        for (const auto& [left, right] : pairs) {
            // fmt::print("== Pair {} ==\n", pair_n);
            auto found = false;
            const auto rv = compare(left, right, found);
            if (rv) {
                sum += (pair_n);
            }
            // fmt::print("\n");
            pair_n++;
        }
        fmt::print("{}\n", sum);
        assert(sum == 6623);
    }
    {
        const auto first_divider = "[[2]]";
        const auto second_divider = "[[6]]";
        auto packets = std::vector<std::string>{first_divider, second_divider};
        for (const auto& [left, right] : pairs) {
            packets.push_back(left);
            packets.push_back(right);
        }
        ranges::actions::sort(packets, [](const auto& left, const auto& right) {
            auto found = false;
            return compare(left, right, found);
        });
        const auto get_divider_index = [&](const auto divider) {
            auto result = 0;
            if (const auto it = ranges::find(packets, divider); it != ranges::cend(packets)) {
                result = static_cast<int>(it - ranges::cbegin(packets) + 1);
            }
            return result;
        };
        const auto result = get_divider_index(first_divider) * get_divider_index(second_divider);
        assert(result == 23049);
        fmt::print("{}\n", result);
    }
}
