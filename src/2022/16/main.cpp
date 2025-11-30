#include "input.h"

#include <boost/functional/hash.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <cassert>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using ValvesStr = std::map<std::string, std::vector<std::string>>;
using RatesStr = std::vector<std::pair<std::string, int>>;
using ValvesAndRatesStr = std::pair<ValvesStr, RatesStr>;
using Valves = std::vector<std::vector<int>>;
using Rates = std::vector<int>;
using ValvesAndRates = std::pair<Valves, Rates>;
using OpenedValves = std::set<int>;
using State = std::tuple<int, OpenedValves, int, bool>;
using States = std::unordered_map<State, int>;

template<>
struct std::hash<State> {
    [[nodiscard]] auto operator()(const State& state) const -> std::size_t
    {
        auto result = std::size_t{};
        const auto& [valve, opened_valves, time_left, with_elephant] = state;
        boost::hash_combine(result, boost::hash_value(valve));
        boost::hash_combine(result, boost::hash_value(opened_valves));
        boost::hash_combine(result, boost::hash_value(time_left));
        boost::hash_combine(result, boost::hash_value(with_elephant));
        return result;
    }
};

[[nodiscard]] auto to_index(const RatesStr& rates, const std::string_view s) -> std::size_t
{
    auto result = std::size_t{};
    if (const auto it = ranges::find_if(rates, [&](const auto& r) { return r.first == s; });
        it != ranges::cend(rates)) {
        result = it - ranges::cbegin(rates);
    }
    return result;
}

[[nodiscard]] auto parse_input(const std::string_view input) -> ValvesAndRates
{
    auto result = ValvesAndRates{};
    auto& [valves, rates] = result;
    auto valves_and_rates_str = ValvesAndRatesStr{};
    auto& [valves_str, rates_str] = valves_and_rates_str;
    for (const auto range : input | ranges::views::split('\n')) {
        const auto line = (range | ranges::to<std::string>());
        const auto valve = line.substr(std::size("Valve")).substr(0, 2);
        const auto valves_raw_str = [&] {
            if (const auto pos = line.find("valves"); pos != std::string::npos) {
                return line.substr(pos).substr(std::size("valves"));
            }
            return line.substr(line.find("valve")).substr(std::size("valve"));
        }();
        auto vec = std::vector<std::string>{};
        for (const auto& v : valves_raw_str | ranges::views::split(ranges::views::c_str(", "))) {
            vec.push_back(v | ranges::to<std::string>());
        }
        valves_str.emplace(valve, vec);
        rates_str.emplace_back(valve, std::stoi(line.substr(std::size("Valve XX has flow rate"))));
    }
    const auto first_valve =
        *ranges::find_if(rates_str, [](const auto& r) { return r.first == "AA"; });
    rates_str.erase(ranges::remove(rates_str, first_valve), ranges::end(rates_str));
    auto new_rates_str = RatesStr{first_valve};
    new_rates_str.insert(new_rates_str.end(), rates_str.begin(), rates_str.end());
    for (const auto& r : new_rates_str) {
        rates.push_back(r.second);
    }
    valves = Valves{std::size(rates)};
    for (const auto& [key, value] : valves_str) {
        const auto index = to_index(new_rates_str, key);
        auto rates_int = std::vector<int>{};
        ranges::transform(value, std::back_inserter(rates_int), [&](const auto& element) {
            return to_index(new_rates_str, element);
        });
        valves.at(index) = rates_int;
    }
    return result;
}

[[nodiscard]] auto get_max_pressure(
    const ValvesAndRates& valves_rates,
    States& states,
    const int valve,
    OpenedValves opened_valves,
    int time_left,
    const bool with_elephant = false) -> int
{
    auto result = 0;
    const auto& [valves, rates] = valves_rates;
    if (time_left == 0) {
        if (!with_elephant) {
            return 0;
        }
        const auto time_left_with_elephant = 26;
        const auto start_valve = 0;
        return get_max_pressure(
            valves_rates, states, start_valve, opened_valves, time_left_with_elephant);
    }
    const auto state = State{valve, opened_valves, time_left, with_elephant};
    if (states.contains(state)) {
        return states.at(state);
    }
    --time_left;
    for (const auto& v : valves.at(valve)) {
        const auto pressure =
            get_max_pressure(valves_rates, states, v, opened_valves, time_left, with_elephant);
        result = std::max(result, pressure);
    }
    if (const auto rate = rates.at(valve); !opened_valves.contains(valve) && rate > 0) {
        opened_valves.insert(valve);
        const auto pressure =
            get_max_pressure(valves_rates, states, valve, opened_valves, time_left, with_elephant);
        result = std::max(result, time_left * rate + pressure);
    }
    states.emplace(state, result);
    return result;
}

auto main() -> int
{
    const auto valves_rates = parse_input(Input);
    const auto start_valve = 0;
    auto states = States{};
    auto time_left = 30;
    const auto result1 = get_max_pressure(valves_rates, states, start_valve, {}, time_left);
    time_left = 26;
    states = States{};
    const auto with_elephant = true;
    const auto result2 =
        get_max_pressure(valves_rates, states, start_valve, {}, time_left, with_elephant);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 1754);
    assert(result2 == 2474);
}
