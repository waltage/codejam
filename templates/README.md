# A helper CLI for generating C++ Templates

This utility is designed mainly to configure a CLion directory with all of the 
run configurations pre-populated.

## Installation:
`go install github.com/waltage/codejam/...@latest`

## Usage:
`jamsetup [PROJECT_NAME: string] [N_PROBLEMS: int]`

This will then create a directory named `${PROJECT_NAME}` and fill it with:
- `CMakeLists.txt` with an executable for each problem.  This currently uses C++17
and CMake 3.20
- A pair of files for each problem number: `problem_##.cpp` and `problem_##.stdin.txt`
  (the latter to be used with test cases)
- A set of project, CMAKE, and CMake Run configurations for use with CLion.  

When opening the resulting directory with CLion, open the CMakeLists.txt file first, 
and load the project; After the initial build, you should then have run configurations already
populated and ready to go.