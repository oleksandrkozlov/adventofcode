#include "input.h"

#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct Folder {
    explicit Folder(std::string n = {}, Folder* p = {})
        : name{std::move(n)}
        , parent{p}
    {
    }

    std::string name;
    std::map<std::string, int> files;
    std::map<std::string, Folder> childFolders;
    Folder* parent = nullptr;
};

auto operator<(const Folder& lhs, const Folder& rhs) -> bool
{
    return (lhs.name < rhs.name);
}

auto result1 = 0;

auto printFolders(Folder& folders, std::string shift, std::vector<int>& allDirSizes) -> int
{
    auto result = 0;
    for (auto& [folderName, childFolder] : folders.childFolders) {
        const auto newShift = shift + "  ";
        auto thisDirSize = printFolders(childFolder, newShift, allDirSizes);
        for (const auto& [fileName, fileSize] : childFolder.files) {
            thisDirSize += fileSize;
            // fmt::print("{}- {} (file, size={})\n", newShift, fileName, fileSize);
        }
        result += thisDirSize;
        if (thisDirSize <= 100000) {
            result1 += thisDirSize;
        }
        allDirSizes.push_back(thisDirSize);
        // fmt::print("{}- {} (dir, size={})\n", shift, folderName, thisDirSize);
    }
    return result;
};

auto getTotalSizes(const std::string_view input, std::vector<int>& allDirSizes) -> int
{
    using namespace std::literals;
    constexpr auto DirCmd = "$ cd "sv;
    constexpr auto Dir = "dir "sv;
    auto rootFolder = Folder{};
    auto thisFolder = &rootFolder;
    thisFolder->childFolders.emplace("/", Folder{"/", thisFolder});
    for (const auto range : input | ranges::views::split('\n')) {
        const auto line = range | ranges::to<std::string>();
        if (line.starts_with('$')) {
            if (line.starts_with(DirCmd)) {
                if (const auto changeDir = line.substr(std::size(DirCmd)); changeDir != "..") {
                    thisFolder = &thisFolder->childFolders.at(changeDir);
                    continue;
                }
                thisFolder = thisFolder->parent;
            }
            continue;
        }
        if (line.starts_with(Dir)) {
            const auto dir = line.substr(std::size(Dir));
            thisFolder->childFolders.emplace(dir, Folder{dir, thisFolder});
            continue;
        }
        auto index = std::size_t{};
        const auto fileSize = std::stoi(line, &index);
        const auto fileName = line.substr(index + 1);
        thisFolder->files.emplace(fileName, fileSize);
    }
    return printFolders(rootFolder, "", allDirSizes);
}

int main()
{
    auto allDirSizes = std::vector<int>{};
    const auto allSpace = 70000000;
    const auto updateSize = 30000000;
    const auto busySpace = getTotalSizes(Input, allDirSizes);
    const auto freeSpace = allSpace - busySpace;
    const auto needSpace = updateSize - freeSpace;
    auto range = (allDirSizes |= ranges::actions::sort) |
        ranges::views::filter([&](const auto size) { return size >= needSpace; }) |
        ranges::views::take(1);
    const auto result2 = *ranges::begin(range);
    fmt::print("{}\n{}\n", result1, result2);
    assert(result1 == 919137);
    assert(result2 == 2877389);
}
