Advent of Code
==============

[Advent of Code](https://adventofcode.com) is an Advent calendar of programming puzzles solved in C++.

### Build

```
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

### Puzzle setup

* `./aoc.sh` updates `CMakeLists.txt`, downloads `input`, creates `main.cpp`, builds, and runs it.

* Set `AOC_SESSION`, then run:
```
./aoc.sh <year> <day>
```
For example:
```
./aoc.sh 2015 1
```
Result:
```
src/2015
├── 01
│   ├── input
│   └── main.cpp
└── CMakeLists.txt
```

### Dependencies

* [C++26](https://en.cppreference.com/w/cpp/26.html)
* [CMake](https://cmake.org)
* [boost](https://www.boost.org)
* [range-v3](https://ericniebler.github.io/range-v3)
* [{fmt}](https://fmt.dev)
* [GMP](https://gmplib.org) (optional)
* [Symengine](https://symengine.org) (optional)
* [z3](https://github.com/Z3Prover/z3) (optional)
