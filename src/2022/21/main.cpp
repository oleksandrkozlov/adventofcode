#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>
#include <symengine/expression.h>
#include <symengine/solve.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

using Numbers = std::unordered_map<std::string, std::int64_t>;
using Operations = std::unordered_map<std::string, std::string>;

[[nodiscard]] auto parse_input(const std::string_view input, const std::size_t part)
    -> std::pair<Numbers, Operations>
{
    auto result = std::pair<Numbers, Operations>{};
    auto& [numbers, operations] = result;
    for (const auto range : input | ranges::views::split('\n')) {
        const auto row = range | ranges::views::split(ranges::views::c_str(": "));
        const auto begin = ranges::begin(row);
        const auto name = *begin | ranges::to<std::string>();
        auto value = *ranges::next(begin) | ranges::to<std::string>();
        const auto chars = std::array{'+', '-', '*', '/'};
        if (ranges::none_of(chars, [&](const auto c) { return value.contains(c); })) {
            if (part == 1 || (part == 2 && name != "humn")) {
                numbers.emplace(name, std::stoi(value));
            } else {
                operations.emplace(name, "x");
            }
        } else {
            if (part == 2 && name == "root") {
                for (const auto c : chars) {
                    if (value.contains(c)) {
                        ranges::replace(value, c, '=');
                        break;
                    }
                }
            }
            operations.emplace(name, value);
        }
    }
    return result;
}

auto get_yell_number(std::pair<Numbers, Operations> input, const std::size_t part) -> std::int64_t
{
    auto [numbers, operations] = input;
    const auto get_pair_numbers =
        [&](const auto monkeys) -> std::optional<std::pair<std::int64_t, std::int64_t>> {
        const auto begin = ranges::begin(monkeys);
        const auto first_monkey = *begin | ranges::to<std::string>();
        const auto second_monkey = *ranges::next(begin) | ranges::to<std::string>();
        if (numbers.contains(first_monkey) && numbers.contains(second_monkey)) {
            return {{numbers.at(first_monkey), numbers.at(second_monkey)}};
        }
        return {};
    };
    const auto split_by = [](const auto delimeter) {
        return ranges::views::split(ranges::views::c_str(delimeter));
    };
    const auto get_pair = [&](const auto& operation, const auto delimeter) {
        return get_pair_numbers(operation | split_by(delimeter));
    };
    auto prev_size = std::size_t{};
    auto curr_size = std::size(operations);
    while (prev_size != curr_size) {
        prev_size = std::size(operations);
        for (auto it = std::begin(operations), it1 = it; it != std::end(operations); it = it1) {
            ++it1;
            const auto& [name, operation] = *it;
            if (name == "humn" && name == "root") {
                continue;
            }
            if (auto delim = " + "; operation.contains(delim)) {
                if (const auto pair = get_pair(operation, delim); pair.has_value()) {
                    const auto [lhs, rhs] = pair.value();
                    numbers.emplace(name, lhs + rhs);
                    operations.erase(it);
                }
            } else if (delim = " - "; operation.contains(delim)) {
                if (const auto pair = get_pair(operation, delim); pair.has_value()) {
                    const auto [lhs, rhs] = pair.value();
                    numbers.emplace(name, lhs - rhs);
                    operations.erase(it);
                }
            } else if (delim = " * "; operation.contains(delim)) {
                if (const auto pair = get_pair(operation, delim); pair.has_value()) {
                    const auto [lhs, rhs] = pair.value();
                    numbers.emplace(name, lhs * rhs);
                    operations.erase(it);
                }
            } else if (delim = " / "; operation.contains(delim)) {
                if (const auto pair = get_pair(operation, delim); pair.has_value()) {
                    const auto [lhs, rhs] = pair.value();
                    numbers.emplace(name, lhs / rhs);
                    operations.erase(it);
                }
            }
        }
        curr_size = std::size(operations);
    }
    if (part == 1) {
        return numbers.at("root");
    }
    assert(part == 2);
    assert(operations.contains("root"));
    auto operation = operations.at("root");
    while (true) {
        auto replaced_operation = false;
        auto replaced_number = false;
        for (const auto& [name, v] : operations) {
            if (operation.contains(name)) {
                operation.replace(operation.find(name), name.size(), fmt::format("({})", v));
                replaced_operation = true;
            }
        }
        for (const auto& [name, v] : numbers) {
            if (operation.contains(name)) {
                operation.replace(operation.find(name), name.size(), std::to_string(v));
                replaced_number = true;
            }
        }
        if (!replaced_number && !replaced_operation) {
            break;
        }
    }
    assert(operation.contains('='));
    ranges::replace(operation, '=', '-');
    const auto expression = SymEngine::Expression(operation);
    const auto result = SymEngine::solve(expression, SymEngine::symbol("x"))->__str__();
    return std::stol(result.substr(1, std::size(result) - 2));
}

auto main() -> int
{
    const auto input1 = parse_input(Input, 1);
    const auto result1 = get_yell_number(input1, 1);
    const auto input2 = parse_input(Input, 2);
    const auto result2 = get_yell_number(input2, 2);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 286698846151845);
    assert(result2 == 3759566892641);
}
