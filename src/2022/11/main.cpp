#include "input.h"

#include <gmpxx.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

[[nodiscard]] auto get_monkey_busines_level(const std::size_t part) -> mpz_class
{
    auto items = std::vector<std::queue<mpz_class>>{};
    auto divs = std::vector<int>{};
    auto operations = std::vector<std::string>{};
    auto true_to = std::vector<int>{};
    auto false_to = std::vector<int>{};
    for (auto range : Input | ranges::view::split(ranges::view::c_str("\n\n"))) {
        for (auto monkey_rng : range | ranges::view::split('\n')) {
            auto monkey = monkey_rng | ranges::to<std::string>();
            if (monkey.starts_with("  Starting items:")) {
                auto queue = std::queue<mpz_class>{};
                auto items_str = monkey.substr(std::size("  Starting items:"));
                for (auto item : items_str | ranges::view::split(ranges::view::c_str(", "))) {
                    queue.push(mpz_class{item | ranges::to<std::string>()});
                }
                items.push_back(queue);
            } else if (monkey.starts_with("  Operation: new =")) {
                auto operation = monkey.substr(std::size("  Operation: new ="));
                operations.push_back(operation);
            } else if (monkey.starts_with("  Test: divisible by")) {
                auto div_by = std::stoi(monkey.substr(std::size("  Test: divisible by")));
                divs.push_back(div_by);
            } else if (monkey.starts_with("    If true: throw to monkey")) {
                auto to = std::stoi(monkey.substr(std::size("    If true: throw to monkey")));
                true_to.push_back(to);
            } else if (monkey.starts_with("    If false: throw to monkey")) {
                auto to = std::stoi(monkey.substr(std::size("    If false: throw to monkey")));
                false_to.push_back(to);
            }
        }
    }
    auto lcm = mpz_class{1};
    for (auto x : divs) {
        lcm *= x;
    }
    auto check_count = std::vector<mpz_class>(items.size(), mpz_class{0});
    const auto rounds = part == 1 ? 20 : 10000;
    for (auto round = 1; round <= rounds; ++round) {
        for (auto i = std::size_t{0}; i < items.size(); ++i) {
            while (!items.at(i).empty()) {
                auto item = items.at(i).front();
                ++check_count.at(i);
                items.at(i).pop();
                auto operation = operations.at(i);
                auto found = operation.find("old");
                operation.replace(found, std::size("old") - 1, item.get_str());
                if (operation.ends_with("old")) {
                    found = operation.find("old");
                    operation.replace(found, std::size("old") - 1, item.get_str());
                }
                auto vals = operation | ranges::view::split(' ');
                auto lhs = mpz_class{*vals.begin() | ranges::to<std::string>()};
                auto rhs = mpz_class{*ranges::next(vals.begin(), 2) | ranges::to<std::string>()};
                auto sign = *ranges::next(vals.begin(), 1) | ranges::to<std::string>();
                auto res = [&] {
                    if (sign == "+") {
                        return mpz_class{lhs + rhs};
                    }
                    return mpz_class{lhs * rhs};
                }();
                if (part == 1) {
                    res = res / 3;
                } else {
                    assert(part == 2);
                    res %= lcm;
                }
                auto next_monkey = [&] {
                    if (res % divs.at(i) == 0) {
                        return true_to.at(i);
                    }
                    return false_to.at(i);
                }();
                items.at(next_monkey).push(res);
            }
        }
    }
    std::sort(check_count.begin(), check_count.end(), std::greater<mpz_class>());
    auto result = mpz_class{1};
    for (int i = 0; i < 2; ++i) {
        result *= check_count.at(i);
    }
    return result;
}

int main()
{
    const auto result1 = get_monkey_busines_level(1);
    const auto result2 = get_monkey_busines_level(2);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == 69918);
    assert(result2 == 19573408701);
}
