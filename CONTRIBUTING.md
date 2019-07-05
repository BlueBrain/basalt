# Contribution Guide

We would love for you to contribute to the basalt project and help make it better than it is today.
As a contributor, here are the guidelines we would like you to follow:

 - [Question or Problem?](#got-a-question)
 - [Issues and Bugs](#found-a-bug)
 - [Feature Requests](#missing-a-feature)
 - [Submissions](#submission-guidelines)
 - [Development Guidelines](#development)
 - [Release Procedure](#release)

# Got a Question?

Please do not hesitate to raise an issue on [github project page][github].

# Found a Bug?

If you find a bug in the source code, you can help us by [submitting an issue](#issues) to our [GitHub Repository][github]. Even better, you can [submit a Pull Request](#pull-requests) with a fix.

#  Missing a Feature?

You can *request* a new feature by [submitting an issue](#issues) to our GitHub Repository. If you would like to *implement* a new feature, please submit an issue with a proposal for your work first, to be sure that we can use it.

Please consider what kind of change it is:

* For a **Major Feature**, first open an issue and outline your proposal so that it can be
discussed. This will also allow us to better coordinate our efforts, prevent duplication of work,
and help you to craft the change so that it is successfully accepted into the project.
* **Small Features** can be crafted and directly [submitted as a Pull Request](#pull-requests).

# Submission Guidelines

## Issues

Before you submit an issue, please search the issue tracker, maybe an issue for your problem already exists and the
discussion might inform you of workarounds readily available.

We want to fix all the issues as soon as possible, but before fixing a bug we need to reproduce and confirm it. In order to reproduce bugs we will need as much information as possible, and preferably with an example.

## Pull Requests

When you wish to contribute to the code base, please consider the following guidelines:

* Make a [fork](https://guides.github.com/activities/forking/) of this repository.
* Make your changes in your fork, in a new git branch:

     ```shell
     git checkout -b my-fix-branch master
     ```
* Create your patch, **including appropriate test cases**.
* Enable CMake environment variables `Basalt_FORMATTING`
  and `Basalt_PRECOMMIT` to ensure that your contribution complies
  with the coding conventions of this project.
  Please check the coding [conventions](#coding-conventions) for more information.
* Run the full test suite, and ensure that all tests pass.
* Commit your changes using a descriptive commit message.

     ```shell
     git commit -a
     ```
* Push your branch to GitHub:

    ```shell
    git push origin my-fix-branch
    ```
* In GitHub, send a Pull Request to the `master` branch of the upstream repository of the relevant component.
* If we suggest changes then:
  * Make the required updates.
  * Re-run the test suites to ensure tests are still passing.
  * Rebase your branch and force push to your GitHub repository (this will update your Pull Request):

       ```shell
        git rebase master -i
        git push -f
       ```

That’s it! Thank you for your contribution!

### After your pull request is merged

After your pull request is merged, you can safely delete your branch and pull the changes from the main (upstream)
repository:

* Delete the remote branch on GitHub either through the GitHub web UI or your local shell as follows:

    ```shell
    git push origin --delete my-fix-branch
    ```
* Check out the master branch:

    ```shell
    git checkout master -f
    ```
* Delete the local branch:

    ```shell
    git branch -D my-fix-branch
    ```
* Update your master with the latest upstream version:

    ```shell
    git pull --ff upstream master
    ```

[github]: https://github.com/tristan0x/basalt

# Development

During the development phase, it is recommended to install basalt in
editable mode with pip (see pip install `--editable` option).

## Coding conventions

Make sure to enable both `Basalt_FORMATTING` and `Basalt_PRECOMMIT`
CMake variables to ensure that your contributions follow the coding conventions of this project:

```cmake
cmake -DBasalt_FORMATTING:BOOL=ON -DBasalt_PRECOMMIT:BOOL=ON <path>
```

The first variable provides the following additional targets to format
C, C++, and CMake files:

```
make clang-format cmake-format
```

The second option activates Git hooks that will discard commits that
do not comply with coding conventions of this project. These 2 CMake variables require additional utilities:

* [ClangFormat 7](https://releases.llvm.org/7.0.0/tools/clang/docs/ClangFormat.html)
* [cmake-format](https://github.com/cheshirekow/cmake_format)
* [pre-commit](https://pre-commit.com/)

clang-format can be installed on Linux thanks
to [LLVM apt page](http://apt.llvm.org/). On MacOS, there is a
[brew recipe](https://gist.github.com/ffeu/0460bb1349fa7e4ab4c459a6192cbb25)
to install clang-format 7. _cmake-format_ and _pre-commit_ utilities can be installed with *pip*.

## Wrap up

When your contribution is ready and tests pass, then you can
execute the `dev/travis-build` script to foresee issues in the
Python package that may happen during the continuous integration
process later on.

## Memory Leaks and clang-tidy

Additionally, to perform memory checking over unit-tests with valgrind,
C++ and CMake code formatting checks, as well as C++ static analysis
check at once, execute the script as follow:
`CHECK_NATIVE=yes dev/travis-build`

To perform the checks manually, run the corresponding commands from
cmake build directory:

* memory check: `ctest -VV --output-on-failure -T memcheck`
* C++ code formatting check: `make check-clang-format`
  (`make clang-format` to fix it)
* CMake code formatting check: `make check-cmake-format`
  (`make cmake-format` to fix it)
* C++ static analysis check: `make clang-tidy`

If you want to enable `clang-tidy` checks with CMake, make sure to have `CMake >= 3.5` and use following cmake option:

```
cmake .. -DENABLE_CLANG_TIDY=ON
```

# Release

1. Build and test from scratch: `tox -r`
2. Run benchmarks in `tests/benchmarks` and ensure there is no regression.
3. Create a git tag.
4. Create source distribution: `python setup.py sdist`
5. Test installation with pip within a Docker container:
   `dev/check-source-distribution dist/basalt-VERSION.tar.gz`
6. Build documentation and update the *gh-pages* accordingly.
7. Upload source distribution on pypi:
   `twine upload -s basalt-VERSION.tar.gz`
8. Create release on GitHub project.
