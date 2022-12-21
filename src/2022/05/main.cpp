#include "input.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

std::vector<std::string> split_string_by(const std::string& str, const char delimeter)
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};
    for (std::string line; std::getline(ss, line, delimeter);)
        result.push_back(line);

    return result;
}

// [W] [V]     [P]
// [B] [T]     [C] [B]     [G]
// [G] [S]     [V] [H] [N] [T]
// [Z] [B] [W] [J] [D] [M] [S]
// [R] [C] [N] [N] [F] [W] [C]     [W]
// [D] [F] [S] [M] [L] [T] [L] [Z] [Z]
// [C] [W] [B] [G] [S] [V] [F] [D] [N]
// [V] [G] [C] [Q] [T] [J] [P] [B] [M]
//  1   2   3   4   5   6   7   8   9
auto make_input() -> std::vector<std::stack<char>>
{
    auto result = std::vector<std::stack<char>>{};

    auto s1 = std::stack<char>{};
    auto s2 = std::stack<char>{};
    auto s3 = std::stack<char>{};
    auto s4 = std::stack<char>{};
    auto s5 = std::stack<char>{};
    auto s6 = std::stack<char>{};
    auto s7 = std::stack<char>{};
    auto s8 = std::stack<char>{};
    auto s9 = std::stack<char>{};

    s1.push('V');
    s1.push('C');
    s1.push('D');
    s1.push('R');
    s1.push('Z');
    s1.push('G');
    s1.push('B');
    s1.push('W');

    s2.push('G');
    s2.push('W');
    s2.push('F');
    s2.push('C');
    s2.push('B');
    s2.push('S');
    s2.push('T');
    s2.push('V');

    s3.push('C');
    s3.push('B');
    s3.push('S');
    s3.push('N');
    s3.push('W');

    s4.push('Q');
    s4.push('G');
    s4.push('M');
    s4.push('N');
    s4.push('J');
    s4.push('V');
    s4.push('C');
    s4.push('P');

    s5.push('T');
    s5.push('S');
    s5.push('L');
    s5.push('F');
    s5.push('D');
    s5.push('H');
    s5.push('B');

    s6.push('J');
    s6.push('V');
    s6.push('T');
    s6.push('W');
    s6.push('M');
    s6.push('N');

    s7.push('P');
    s7.push('F');
    s7.push('L');
    s7.push('C');
    s7.push('S');
    s7.push('T');
    s7.push('G');

    s8.push('B');
    s8.push('D');
    s8.push('Z');

    s9.push('M');
    s9.push('N');
    s9.push('Z');
    s9.push('W');

    result.push_back(s1);
    result.push_back(s2);
    result.push_back(s3);
    result.push_back(s4);
    result.push_back(s5);
    result.push_back(s6);
    result.push_back(s7);
    result.push_back(s8);
    result.push_back(s9);

    return result;
}

[[nodiscard]] auto get_top_boxes_while_moving_one_by_one(
    std::vector<std::stack<char>> boxes,
    const std::vector<std::string>& moves) -> std::string
{
    auto result = std::string{};
    for (auto move_str : moves) {
        move_str = move_str.substr(5);
        auto sub = std::size_t{0};
        auto n_boxes = std::stoi(move_str, &sub);
        move_str = move_str.substr(sub + 6);
        auto from = std::stoi(move_str, &sub);
        move_str = move_str.substr(sub + 4);
        auto to = std::stoi(move_str);
        auto& stack_from = boxes.at(from - 1);
        auto& stack_to = boxes.at(to - 1);
        for (auto i = 0; i < n_boxes; ++i) {
            auto box = stack_from.top();
            stack_from.pop();
            stack_to.push(box);
        }
    }
    for (const auto& s : boxes) {
        auto box = s.top();
        result += box;
    }
    return result;
}

[[nodiscard]] auto get_top_boxes_while_move_multiple_at_once(
    std::vector<std::stack<char>> boxes,
    const std::vector<std::string>& moves) -> std::string
{
    auto result = std::string{};
    for (auto move_str : moves) {
        move_str = move_str.substr(5);
        auto sub = std::size_t{0};
        auto n_boxes = std::stoi(move_str, &sub);
        move_str = move_str.substr(sub + 6);
        auto from = std::stoi(move_str, &sub);
        move_str = move_str.substr(sub + 4);
        auto to = std::stoi(move_str);
        auto& stack_from = boxes.at(from - 1);
        auto& stack_to = boxes.at(to - 1);
        auto box_group = std::vector<char>{};
        for (auto i = 0; i < n_boxes; ++i) {
            auto box = stack_from.top();
            stack_from.pop();
            box_group.push_back(box);
        }
        for (int it = static_cast<int>(box_group.size()) - 1; it >= 0; --it) {
            stack_to.push(box_group.at(it));
        }
    }
    for (const auto& s : boxes) {
        auto box = s.top();
        result += box;
    }
    return result;
}

[[nodiscard]] auto get_top_boxes(
    const std::vector<std::stack<char>>& boxes,
    const std::vector<std::string>& moves,
    const std::size_t part) -> std::string
{
    if (part == 1) {
        return get_top_boxes_while_moving_one_by_one(boxes, moves);
    }
    assert(part == 2);
    {
        return get_top_boxes_while_move_multiple_at_once(boxes, moves);
    }
}

auto main() -> int
{
    const auto boxes = make_input();
    const auto moves = split_string_by(std::string{Input}, '\n');
    const auto result1 = get_top_boxes(boxes, moves, 1);
    const auto result2 = get_top_boxes(boxes, moves, 2);
    std::cout << result1 << std::endl << result2 << std::endl;
    assert(result1 == "TBVFVDZPN");
    assert(result2 == "VLCWHTDSZ");
}
