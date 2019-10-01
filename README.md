![basalt logo](https://github.com/BlueBrain/basalt/raw/master/doc/source/_static/basalt-logo-400.png)

# Basalt - Graph Storage API for C++ and Python

Basalt is a graph storage API powered by RocksDB persistent
key-value store for fast storage like NVMe technologies.

[![Build Status](https://api.travis-ci.com/BlueBrain/basalt.svg?token=p3ijqmiSc83uPHF74Ay8&branch=master)](https://travis-ci.org/BlueBrain/basalt)

Documentation is currently hosted on GitHub: [https://bluebrain.github.io/basalt]

# Development stage

The C++ and Python APIs of Basalt are stable already, but substantial additions might come in the future. Thus this library development status is still beta.

# Usage

## Python

### Graph Topology API

```python
class PLInfluences(basalt.GraphTopology):
    """A directed graph where vertices are programming languages.
    """
    directed(True)

    class Vertex(Enum):
        LANGUAGE = 1

    # Declare a vertex type
    vertex("language", Vertex.LANGUAGE)
    # Declare a directed edge between 2 programming languages
    # to represent how they relate.
    edge(Vertex.LANGUAGE, Vertex.LANGUAGE, name="influenced", plural="influenced")

    @classmethod
    def load_from_dbpedia(cls):
        # [...]

g = PLInfluences.load_from_dbpedia("/path/on/disk")
# Iterate over all vertices of type "languages"
for language in g.languages:
  print(language.id, language.data())
  # Iterate over all vertices connected to vertex `language`
  # through the `influenced` edge type.
  for influenced in language.influenced:
    print("  ", influenced.data())
```

### Low-level Python bindings

```python
# Load or initialize a graph on disk
g = basalt.UndirectedGraph("/path/on/disk")
# Add one vertex of type 0 and identifier 1
g.vertices.add((0, 1))
# Insert 10 vertices at once
# (10, 0), (10, 1), ... (10, 10)
g.vertices.add(numpy.full((10,), 1, dtype=numpy.int32), # types
               numpy.arange(10, dtype=numpy.int64)) # ids
# Connect 2 vertices
g.edges.add((0, 1), (1, 0))
# Connect vertex (0, 1) to several vertices at once
# (0,1)->(1,0), (0,1)->(1,1), ... (0,1)->(1,9)
g.edges.add((0, 1),
            numpy.full((9,), 1, dtype=numpy.int32),
            numpy.arange(9, dtype=numpy.int64)
# Commit changes on disk
g.commit()
```

## C++ API

```cpp
// Load or initialize a graph on disk
basalt::UndirectedGraph g("/path/on/disk");
// Add one vertex of type 0 and identifier 1
g.vertices().insert({0, 1});
// Add one vertex of type 0 and identifier 2
g.vertices().insert({0, 2});
// Iterate over vertices
for (const auto& vertex: g.vertices()) {
  std::clog << vertex << '\n';
}
// Connect both vertices
g.edges().insert({0, 1}, {0, 2}));
for (const auto& edge: g.edges()) {
  std::clog << edge.first << " -> " << edge.second << '\n';
}
// Commit changes on disk
g.commit();
```

# Installation

## C++ API

### Conan package

This repository provides a [Conan](https://conan.io/) package to ease integration into your existing projects.

### CMake

It is also possible to build and install the library using CMake, see [build section](#manual-build-and-installation-instructions) below.

## Python API

### Pypi

Python bindings of Basalt are available on [Pypi](https://pypi.org/simple/basalt).

### Blue Brain 5 supercomputer

Basalt is currently released as module on Blue Brain 5 supercomputer:

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
```

# Manual build and installation instructions

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
  * [wheel](https://www.python.org/dev/peps/pep-0427/): `pip3 install wheel; python3 setup.py bdist_wheel`
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
├── cmake
│   └── hpc-coding-conventions git module for C++ code guidelines
├── dev ...................... development related scripts
├── doc ...................... sphinx documentation source code
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

If you want to improve the project or you see any issue, every contribution is welcome.
Please check [contribution guidelines](CONTRIBUTING.md) for more information.
