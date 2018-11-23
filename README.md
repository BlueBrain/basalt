# Basalt - C++11 Graph Storage library

# Build instruction

[CMake](https://cmake.org) is required to build this library.

```sh
cd /path/to/basalt
mkdir build
pushd build
cmake ..
make
```

# Files Layout

* `include/` directory: public headers of the library
* `src/basalt/` directory: source code of the library
* `test/unit/` directory: C++ unit-tests using 
  [Catch2](https://github.com/catchorg/Catch2) library
* `cmake/` directory: Additional CMake scripts
* `dev/` directory: development utilities

# Embedded library

External libraries are including either by copy/paste or git submodules
in `src/third_party` directory.

* [Catch2]((https://github.com/catchorg/Catch2)):
  modern, C++-native, header-only, test framework for unit-tests, TDD
  and BDD unit-test library.
* [SpdLog](https://github.com/gabime/spdlog): Fast C++ logging library.
* [fmt](https://github.com/fmtlib/fmt): A modern formatting library
  **(not part of CMake build yet)**

# Contributing

Contributions are welcome, via GitHub pull-requests and bug tracker.

## pull-requests

Please make sure to execute `dev/clang-format.sh` and `dev/clang-tidy.sh`
before submitting a patch.
