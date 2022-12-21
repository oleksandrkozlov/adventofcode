#include "input.h"

#include <cstddef>
#include <iostream>
#include <numeric>
#include <ranges>

namespace {

namespace views = std::views;

constexpr auto sum(auto&& c, auto action) noexcept
{
    return std::accumulate(c.begin(), c.end(), 0U, action);
}

constexpr auto splitByGame = views::split('\n');

enum class Shape {
    Rock,
    Paper,
    Scissors,
};

enum class Result {
    Lose,
    Draw,
    Win,
};

using enum Shape;
using enum Result;

// clang-format off
[[nodiscard]] constexpr auto toShape(const char shape) noexcept -> Shape
{
    if (shape == 'A') { return Rock; }
    if (shape == 'B') { return Paper; }
    if (shape == 'C') { return Scissors; }
    if (shape == 'X') { return Rock; }
    if (shape == 'Y') { return Paper; }
    return Scissors;
}

[[nodiscard]] constexpr auto toNeededResult(const char result) noexcept -> Result
{
    if (result == 'X') { return Lose; }
    if (result == 'Y') { return Draw; }
    return Win;
}

[[nodiscard]] constexpr auto myShape(const Shape shape, Result result) noexcept -> Shape
{
    if ((shape == Rock && result == Lose) ||
        (shape == Paper && result == Win) ||
        (shape == Scissors && result == Draw)) {
        return Scissors;
    }
    if ((shape == Rock && result == Draw) ||
        (shape == Paper && result == Lose) ||
        (shape == Scissors && result == Win)) {
        return Rock;
    }
    return Paper;
}

[[nodiscard]] constexpr auto operator<=>(const Shape me, const Shape opponent) noexcept -> Result
{
    if (me == opponent) {
        return Draw;
    }
    if (((me == Rock) && (opponent == Paper)) ||
        ((me == Paper) && (opponent == Scissors)) ||
        ((me == Scissors) && (opponent == Rock))) {
        return Lose;
    }
    return Win;
}

[[nodiscard]] constexpr auto resultPoints(const Result result) noexcept -> std::size_t
{
    if (result == Lose) { return 0U; }
    if (result == Draw) { return 3U; }
    return 6U;
}

[[nodiscard]] constexpr auto shapePoints(const Shape shape) noexcept -> std::size_t
{
    if (shape == Rock) { return 1U; }
    if (shape == Paper) { return 2U; }
    return 3U;
}
// clang-format on

[[nodiscard]] constexpr auto myPoints(const Shape me, const Shape opponent) noexcept -> std::size_t
{
    return resultPoints(me <=> opponent) + shapePoints(me);
}

constexpr auto toGame1 = views::transform([](auto&& v) {
    const auto game = std::string_view{v.begin(), v.end()};
    return std::make_pair(toShape(game.at(0)), toShape(game.at(2)));
});

constexpr auto toGame2 = views::transform([](auto&& v) {
    const auto game = std::string_view{v.begin(), v.end()};
    return std::make_pair(toShape(game.at(0)), toNeededResult(game.at(2)));
});

[[nodiscard]] constexpr auto getPoints(const std::size_t part) noexcept -> std::size_t
{
    if (part == 1) {
        return sum(Input | splitByGame | toGame1, [](auto points, auto game) {
            const auto [opponent, me] = game;
            return points + myPoints(me, opponent);
        });
    } else {
        return sum(Input | splitByGame | toGame2, [](auto points, auto game) {
            const auto [opponent, neededResult] = game;
            const auto me = myShape(opponent, neededResult);
            return points + myPoints(me, opponent);
        });
    }
}

} // namespace

int main()
{
    constexpr auto result1 = getPoints(1);
    constexpr auto result2 = getPoints(2);
    std::cout << result1 << std::endl << result2 << std::endl;
    static_assert(result1 == 11841U);
    static_assert(result2 == 13022U);
}
