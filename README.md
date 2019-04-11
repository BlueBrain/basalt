# Basalt - C++11 Graph Storage library

Basic graph database backed by RocksDB key-value storage

[![Build Status](https://api.travis-ci.com/tristan0x/basalt.svg?token=p3ijqmiSc83uPHF74Ay8&branch=master)](https://travis-ci.org/tristan0x/basalt)

# Development stage

This library is still under active development, and is not meant to be used
in a production environment.

# Usage on BlueBrain 5

Basalt is currently released as module on BB5 supercomputer:

```bash
$ module purge
$ . /gpfs/bbp.cscs.ch/apps/hpc/jenkins/config/modules.sh
$ module load py-basalt
$ python3
Python 3.6.3 (default, Oct  3 2017, 07:47:49)
[GCC 6.4.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import basalt
>>> basalt.__version__
'0.2.2'
>>> basalt.__rocksdb_version__
'5.17.2'
>>> basalt.serve_doc()
Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

# Build and installation instructions

## Requirements

* [CMake](https://cmake.org) build system, version 3.5.1 or higher.
* [RocksDB](https://rocksdb.org/), a persistent key-value store,
  version 4.1.1 or higher.
* [Python 3](https://python.org/), version 3.5 or higher.

## Getting the code

This repository grabs a few third-party libraries as *git modules*.
To clone them when you clone basalt, use `git clone --recursive` option.

If you have already cloned basalt, you can get the git submodules with
the command:
`git submodule update --recursive --init`

## Building the library 101

### C++ Library only

To build the basalt C++ shared library and run the tests:
```sh
cd /path/to/basalt
mkdir build
pushd build
cmake ..
CTEST_OUTPUT_ON_FAILURE=1 make all test
```

To install the library:
```sh
pushd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local .
make all install
```

### Python 3 bindings

To build and run the tests:

```sh
cd /path/to/basalt
python3 setup.py test
```

To install the package:
* with _pip_: `pip3 install -U .`
* with _distutils_: `python3 setup.py install`
* to create binary tarballs:
  * most simple: `python3 setup.py bdist`
  * [wheel](https://www.python.org/dev/peps/pep-0427/): `pip3 install wheels; python3 setup.py bdist_wheel`
  * relocatable archive: `python3 setup.py bdist_dumb --relative`

## CMake variables and targets

Main CMake variables:

* `Basalt_FORMATTING:BOOL`: provide the build target `clang-format` to check C++ code formatting
* `Basalt_STATIC_ANALYSIS:BOOL`: provide the build target `clang-tidy` to perform static analysis of the C++ code
* `Basalt_ARCH`: value given to the `-m` compiler option. "native" for instance
* `Basalt_PRECOMMIT:BOOL`: Enable automatic checks before git commits
* `Basalt_CXX_OPTIMIZE:BOOL`: Compile C++ with optimization
* `Basalt_CXX_SYMBOLS:BOOL`: Compile C++ with debug symbols
* `Basalt_CXX_WARNINGS:BOOL=ON`: Compile C++ with warnings

For a more detailed list, please refer to file `CMakeCache.txt` in CMake build directory.

CMake targets:

* `basalt`: build the pure C++ library (without Python bindings)
* `_basalt`: build the C++ library with Python bindings
* `unit-tests`: build a C++ executable testing the C++ pure library
* `all`: build the 3 targets above
* `test`: execute the tests. It is recommended to execute the command `ctest --output-on-failure -VV` instead
* `install`: install the pure C++ library and the CMake configuration required to easily use basalt
  in another CMake project

## Python setuptools commands

Here are the main Python  setuptools commands available.

* `build`: build native library
* `test`: build and test the package. It also executes the C++ unit-tests as well as the code snippets in the Sphinx documentation.
* `install`: install the Python package
* `doctest`: execute the code snippets in the Sphinx documentation
* `build_sphinx`: build the Sphinx documentation

For instance: `python3 setup.py build_sphinx`

# Files Layout

```
├── basalt ................... python code of the package
│   ├── ngv .................. specific API for BBP NGV team
├── cmake
│   └── hpc-coding-conventions git module for C++ code guidelines
├── dev ...................... development related scripts
├── docs ..................... sphinx documentation source code
├── include
│   └── basalt ............... public headers of the C++ library
├── README.md ................ that's me!
├── src
│   ├── basalt ............... C++ library implementation
│   └── third_party .......... C++ libraries (mostly as git modules)
└── tests
    ├── benchmarks ........... scripts to execute before creating a git tag
    ├── py ................... python unit-tests
    └── unit ................. C++ unit-tests using Catch2
```

# Embedded third-parties

External libraries are including either by copy/paste or git submodules
in `src/third_party` directory.

* [Catch2](https://github.com/catchorg/Catch2):
  modern, C++-native, header-only, test framework for unit-tests, TDD
  and BDD unit-test library.
* [fmt](https://github.com/fmtlib/fmt): A modern formatting library
  **(not part of CMake build yet)**
* [pybind11](https://pybind11.rtfd.io): Seamless operability between C++11 and Python
* [SpdLog](https://github.com/gabime/spdlog): Fast C++ logging library.

# Contributing

Contributions are welcome, via GitHub pull-requests and bug tracker.

## Pull Requests

Enable CMake environment variables `Basalt_FORMATTING`
and `Basalt_PRECOMMIT` to ensure that your contribution complies
with the coding conventions of this project.

`cmake -DBasalt_FORMATTING:BOOL=ON -DBasalt_PRECOMMIT:BOOL=ON <path>`

During the development phase, it is recommended to install basalt in
editable mode with pip (see pip install `--editable` option).
When your contribution is ready and tests pass, then you can
execute the `dev/travis-build` script to foresee issues
that may happen during the continuous integration process later on.

Please make sure to execute `dev/check.sh` before submitting a patch in order to:
* run static analysis of C++ code with
  [clang-tidy](http://clang.llvm.org/extra/clang-tidy/)
* run Python static analysis and code formatting with
  [flake8](http://flake8.pycqa.org) and [black](https://github.com/ambv/black).
  Also install [flake8-bugbear](https://github.com/PyCQA/flake8-bugbear)
  *flake8* plugin.

Valgrind memory checker is run on unit-tests by Travis. You can execute it locally
with the *memcheck* CTest action: `ctest -VV --output-on-failure -T memcheck`
