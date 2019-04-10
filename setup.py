import inspect
import os
import os.path as osp
import platform
import re
import subprocess
import sys

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.test import test
from distutils.version import LooseVersion


class lazy_dict(dict):
    """When the value associated to a key is a function, then returns
    the function call instead of the function.
    """

    def __getitem__(self, item):
        value = dict.__getitem__(self, item)
        if inspect.isfunction(value):
            return value()
        return value


def get_sphinx_command():
    """Lazy load of Sphinx distutils command class
    """
    from sphinx.setup_command import BuildDoc

    return BuildDoc


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = osp.abspath(sourcedir)


class CMakeBuild(build_ext):
    user_options = build_ext.user_options + [
        ('target=', None, "specify the CMake target to build"),
    ]

    def initialize_options(self):
        self.target = "build_python_ext"
        super(CMakeBuild, self).initialize_options()

    def run(self):
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )

        if platform.system() == "Windows":
            cmake_version = LooseVersion(
                re.search(r"version\s*([\d.]+)", out.decode()).group(1)
            )
            if cmake_version < "3.1.0":
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = osp.abspath(osp.dirname(self.get_ext_fullpath(ext.name)))
        print("CMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir)
        cmake_project = ext.name[1:].capitalize()
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            "-DPYTHON_EXECUTABLE=" + sys.executable,
            "-D{}_USE_pybind11:BOOL=True".format(cmake_project),
            "-D{}_ARCH=native".format(cmake_project),
            "-DCMAKE_BUILD_TYPE=",
        ]

        optimize = "OFF" if self.debug else "ON"
        cmake_args += ["-D" + cmake_project + "_CXX_OPTIMIZE:BOOL=" + optimize]
        build_args = ["--config", optimize, "--target", self.target]

        if platform.system() == "Windows":
            cmake_args += [
                "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(
                    optimize.upper(), extdir
                )
            ]
            if sys.maxsize > 2 ** 32:
                cmake_args += ["-A", "x64"]
            build_args += ["--", "/m"]
        else:
            build_args += ["--", "-j{}".format(max(1, os.cpu_count() - 1))]

        env = os.environ.copy()
        env["CXXFLAGS"] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get("CXXFLAGS", ""), self.distribution.get_version()
        )
        if not osp.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env
        )
        subprocess.check_call(
            ["cmake", "--build", "."] + build_args, cwd=self.build_temp
        )


class PkgInstall(install):
    """Custom distutils command that acts like as a replacement
    for the "install" command.

    It first execute the "install_doc" command before running the
    default "install" command.
    """

    def run(self):
        if not self.skip_build:
            self.run_command("build")
        self.run_command("install_doc")
        super().run()


class PkgTest(test):
    """Custom disutils command that acts like as a replacement
    for the "test" command.

    It first executes the CMake test target, then the standard Python "test" command,
    and finally run the "doctest" command to also validate code snippets in the sphinx
    documentation.
    """

    def run(self):
        super().run()
        self.run_command("test_ext")
        if platform.system() != 'Darwin':
            subprocess.check_call([sys.executable, __file__, "doctest"])

    def reinitialize_command(self, cmd, **kwargs):
        # disable inplace build of extension
        pass


install_requirements = [
    "cached-property>=1.5.1",
    "docopt>=0.6.2",
    "h5py>=2.6.0",
    "humanize>=0.5.1",
    "numpy>=1.11",
    "progress>=1.4",
]
doc_requirements = [
    "exhale",
    "m2r",
    "sphinx-rtd-theme",
    "sphinx<2",
]

setup(
    name="basalt",
    author="Blue Brain Project",
    author_email="bbp-ou-hpc@groupes.epfl.ch",
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Environment :: Console :: Curses",
        "Intended Audience :: Developers",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Topic :: Database :: Database Engines/Servers",
        "Operating System :: MacOS",
        "Operating System :: POSIX :: Linux",
    ],
    description="Graph DB Storage",
    long_description="",
    packages=["basalt", "basalt.ngv"],
    ext_modules=[CMakeExtension("_basalt")],
    cmdclass=lazy_dict(
        build_ext=CMakeBuild,
        test_ext=CMakeBuild,
        install=PkgInstall,
        test=PkgTest,
        install_doc=get_sphinx_command,  # build and copy sphinx documentation in ./basalt/doc
        doctest=get_sphinx_command,  # execute code snippets in documentation
    ),
    package_data={"basalt": ["docs/html/**/*"]},
    zip_safe=False,
    use_scm_version=True,
    install_requires=install_requirements,
    setup_requires=install_requirements + doc_requirements + ["setuptools_scm"],
    entry_points="""
        [console_scripts]
        basalt-cli = basalt.cli:main
    """,
)
