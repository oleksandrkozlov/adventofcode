#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <range/v3/all.hpp>

#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

struct obsidian_price {
    int ore{};
    int clay{};
};

struct geode_price {
    int ore{};
    int obsidian{};
};

struct blueprint {
    int ore{};
    int clay{};
    obsidian_price obsidian;
    geode_price geode;
};

struct Robots {
    auto operator<=>(const Robots& robots) const = default;
    int ore = 1;
    int clay{};
    int obsidian{};
    int geode{};
};

struct Money {
    auto operator<=>(const Money& money) const = default;
    int ore{};
    int clay{};
    int obsidian{};
};

using State = std::tuple<Robots, Robots, Money, int>;
using States = std::unordered_map<State, int>;

template<>
struct std::hash<Robots> {
    [[nodiscard]] auto operator()(const Robots& robots) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto [ore, clay, obsidian, geode] = robots;
        boost::hash_combine(result, std::hash<int>{}(ore));
        boost::hash_combine(result, std::hash<int>{}(clay));
        boost::hash_combine(result, std::hash<int>{}(obsidian));
        boost::hash_combine(result, std::hash<int>{}(geode));
        return result;
    }
};

template<>
struct std::hash<Money> {
    [[nodiscard]] auto operator()(const Money& money) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto [ore, clay, obsidian /*, geode*/] = money;
        boost::hash_combine(result, std::hash<int>{}(ore));
        boost::hash_combine(result, std::hash<int>{}(clay));
        boost::hash_combine(result, std::hash<int>{}(obsidian));
        return result;
    }
};

template<>
struct std::hash<State> {
    [[nodiscard]] auto operator()(const State& state) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto& [old_robots, new_robots, money, time_left] = state;
        boost::hash_combine(result, std::hash<Robots>{}(old_robots));
        boost::hash_combine(result, std::hash<Robots>{}(new_robots));
        boost::hash_combine(result, std::hash<Money>{}(money));
        boost::hash_combine(result, std::hash<int>{}(time_left));
        return result;
    }
};

[[nodiscard]] auto get_blueprints(const std::string_view input) -> std::vector<blueprint>
{
    auto result = std::vector<blueprint>{};
    for (const auto range : input | ranges::views::split('\n')) {
        const auto str = range | ranges::to<std::string>();
        const auto ore_str = "Each ore robot costs"sv;
        const auto clay_str = "Each clay robot costs"sv;
        const auto obsidian_str = "Each obsidian robot costs"sv;
        const auto geode_str = "Each geode robot costs"sv;
        const auto ore_and_str = "ore and"sv;
        const auto geode_ore_str = str.substr(std::size(geode_str) + str.find(geode_str));
        const auto obsidian_ore_str = str.substr(std::size(obsidian_str) + str.find(obsidian_str));
        result.emplace_back(
            std::stoi(str.substr(std::size(ore_str) + str.find(ore_str))),
            std::stoi(str.substr(std::size(clay_str) + str.find(clay_str))),
            obsidian_price{
                std::stoi(obsidian_ore_str),
                std::stoi(obsidian_ore_str.substr(
                    std::size(ore_and_str) + str.find_first_of(ore_and_str)))},
            geode_price{
                std::stoi(geode_ore_str),
                std::stoi(geode_ore_str.substr(
                    std::size(ore_and_str) + str.find_first_of(ore_and_str)))});
    }
    return result;
}

[[nodiscard]] auto money_left(Money money, const blueprint& price, const std::string_view name)
    -> Money
{
    if (name == "geode") {
        money.ore -= price.geode.ore;
        money.obsidian -= price.geode.obsidian;
    } else if (name == "obsidian") {
        money.ore -= price.obsidian.ore;
        money.clay -= price.obsidian.clay;
    } else if (name == "clay") {
        money.ore -= price.clay;
    } else {
        assert(name == "ore");
        money.ore -= price.ore;
    }
    return money;
}

[[nodiscard]] auto create_robots(Robots robots, const std::string_view name) -> Robots
{
    if (name == "geode") {
        ++robots.geode;
    } else if (name == "obsidian") {
        ++robots.obsidian;
    } else if (name == "clay") {
        ++robots.clay;
    } else {
        assert(name == "ore");
        ++robots.ore;
    }
    return robots;
}

[[nodiscard]] auto get_max_geodes(
    const blueprint& price,
    States& states,
    const Robots& old_robots,
    Robots robots,
    Money money,
    int time_left) -> int;

[[nodiscard]] auto build_robot(
    const std::string_view name,
    const int result,
    const blueprint& price,
    States& states,
    const Robots& old_robots,
    const Robots& robots,
    const Money& money,
    const int time_left) -> int
{
    const auto max_geode = get_max_geodes(
        price,
        states,
        robots,
        create_robots(robots, name),
        money_left(money, price, name),
        time_left);
    return std::max(result, max_geode + old_robots.geode);
}

auto ignore_redundant_robots_and_money(
    Robots& robots,
    Money& money,
    const blueprint& price,
    const int time_left) -> void
{
    const auto price_ore = std::max({price.ore, price.clay, price.obsidian.ore, price.geode.ore});
    if (robots.ore >= price_ore) {
        robots.ore = price_ore;
    }
    if (robots.clay >= price.obsidian.clay) {
        robots.clay = price.obsidian.clay;
    }
    if (robots.obsidian >= price.geode.obsidian) {
        robots.obsidian = price.geode.obsidian;
    }
    if (const auto usable_money = time_left * price_ore - robots.ore * (time_left - 1);
        money.ore >= usable_money) {
        money.ore = usable_money;
    }
    if (const auto usable_money = time_left * price.obsidian.clay - robots.clay * (time_left - 1);
        money.clay >= usable_money) {
        money.clay = usable_money;
    }
    if (const auto usable_money =
            time_left * price.geode.obsidian - robots.obsidian * (time_left - 1);
        money.obsidian >= usable_money) {
        money.obsidian = usable_money;
    }
}

auto update_time_and_money(int& time_left, Money& money, const Robots& robots) -> void
{
    --time_left;
    money.ore += robots.ore;
    money.clay += robots.clay;
    money.obsidian += robots.obsidian;
};

[[nodiscard]] auto build_robots(
    int result,
    const blueprint& price,
    States& states,
    const Robots& old_robots,
    const Robots& robots,
    const Money& money,
    const int time_left) -> int
{
    if ((money.ore >= price.geode.ore) && (money.obsidian >= price.geode.obsidian)) {
        result = build_robot("geode", result, price, states, old_robots, robots, money, time_left);
    }
    if ((money.ore >= price.obsidian.ore) && (money.clay >= price.obsidian.clay)) {
        result =
            build_robot("obsidian", result, price, states, old_robots, robots, money, time_left);
    }
    if (money.ore >= price.clay) {
        result = build_robot("clay", result, price, states, old_robots, robots, money, time_left);
    }
    if (money.ore >= price.ore) {
        result = build_robot("ore", result, price, states, old_robots, robots, money, time_left);
    }
    return result;
}

[[nodiscard]] auto do_nothing(
    const int result,
    const blueprint& price,
    States& states,
    const Robots& old_robots,
    const Robots& robots,
    const Money& money,
    const int time_left) -> int
{
    const auto max_geode = get_max_geodes(price, states, robots, robots, money, time_left);
    return std::max(result, max_geode + old_robots.geode);
}

[[nodiscard]] auto get_max_geodes(
    const blueprint& price,
    States& states,
    const Robots& old_robots,
    Robots robots,
    Money money,
    int time_left) -> int
{
    auto result = 0;
    if (time_left == 0) {
        return result;
    }
    const auto state = State{old_robots, robots, money, time_left};
    if (states.contains(state)) {
        return states.at(state);
    }
    ignore_redundant_robots_and_money(robots, money, price, time_left);
    update_time_and_money(time_left, money, old_robots);
    result = build_robots(result, price, states, old_robots, robots, money, time_left);
    result = do_nothing(result, price, states, old_robots, robots, money, time_left);
    states.emplace(state, result);
    return result;
}

auto main() -> int
{
    const auto prices = get_blueprints(Input);
    auto result1 = 0;
    for (auto i = std::size_t{1}; i <= std::size(prices); ++i) {
        auto states = States{};
        const auto max_geodes = get_max_geodes(prices.at(i - 1), states, {}, {}, {}, 24);
        result1 += static_cast<int>(max_geodes * i);
    }
    auto result2 = 1;
    for (auto i = std::size_t{}; i < 3; ++i) {
        auto states = States{};
        const auto max_geodes = get_max_geodes(prices.at(i), states, {}, {}, {}, 32);
        result2 *= max_geodes;
    }
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 1262);
    assert(result2 == 37191);
}
