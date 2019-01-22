 Basalt - C++11 Graph Storage library

Basic graph database backed by RocksDB key-value storage [![Build Status](https://api.travis-ci.com/tristan0x/basalt.svg?token=p3ijqmiSc83uPHF74Ay8&branch=master)](https://travis-ci.org/tristan0x/basalt)

# Development stage

This library is still under active development, and is not meant to be used
in a production environment.

# Documentation

Python API documentation in available in basalt
[wiki](https://github.com/tristan0x/basalt/wiki/Python-API).

# Build and installation instruction

## Requirements

* [CMake](https://cmake.org) build system, version 3.5.1 or higher.
* [RocksDB](https://rocksdb.org/), a persistent key-value store,
  version version 4.1.1 or higher.

## Getting the code

This repository grabs few third-party libraries as *git submodules*.
To clone them when you clone basalt, use `git clone --recursive` option.

If you have already cloned basalt, you can get submodules with command:
`git submodule update --recursive --init`

## C++ Library

To build and run tests:
```sh
cd /path/to/basalt
mkdir build
pushd build
cmake ..
CTEST_OUTPUT_ON_FAILURE=1 make all test
```

To install the library
```sh
pushd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local .
make all install
```

## Python bindings

To build and run tests

```sh
cd /path/to/basalt
python setup.py test
```

To install:
* with _pip_: `pip install -U .`
* with _distutils_: `python setup.py install`
* to create a binary tarballs:
  * most simple: `python setup.py bdist`
  * [wheel](https://www.python.org/dev/peps/pep-0427/): `pip install wheels; python setup.py bdist_wheel`
  * relocatable archive: `python setup.py bdist_dumb --relative`

# Files Layout

* `basalt/` directory: Python code of the package
* `include/` directory: public headers of the library
* `src/basalt/` directory: source code of the library
* `tests/unit/` directory: C++ unit-tests using
  [Catch2](https://github.com/catchorg/Catch2) library
* `tests/py/` directory: unit-tests of the Python package
* `cmake/` directory: Additional CMake scripts
* `dev/` directory: development utilities

# Embedded third-parties

External libraries are including either by copy/paste or git submodules
in `src/third_party` directory.

* [Catch2]((https://github.com/catchorg/Catch2)):
  modern, C++-native, header-only, test framework for unit-tests, TDD
  and BDD unit-test library.
* [fmt](https://github.com/fmtlib/fmt): A modern formatting library
  **(not part of CMake build yet)**
* [pybind11](https://pybind11.rtfd.io): Seamless operability between C++11 and Python
* [SpdLog](https://github.com/gabime/spdlog): Fast C++ logging library.

# Contributing

Contributions are welcome, via GitHub pull-requests and bug tracker.

## pull-requests

Please make sure to execute `dev/check.sh` before submitting a patch in order to:
* format C++ code with
  [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html)
* run static analysis of C++ code with
  [`clang-tidy`](http://clang.llvm.org/extra/clang-tidy/)
* format CMake files with [cmake-format](https://github.com/cheshirekow/cmake_format)
to format your CMake changes.
* run Python static analysis and code formatting with
  [`flake8`](http://flake8.pycqa.org) and [`black`](https://github.com/ambv/black).
  Also install [`flake8-bugbear`](https://github.com/PyCQA/flake8-bugbear)
  *flake8* plugin.
* Valgrind memory checker is run on unit-tests by Travis. You can execute it locally
  with the *memcheck* CTest action: `ctest -VV --output-on-failure -T memcheck`
