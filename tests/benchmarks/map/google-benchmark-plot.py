"""
Process JSON file written by google benchmarks in various ways:

* create one PNG for every benchmark

    %(prog)s plot result.json

* create a Jupyter notebook with the benchmark plots.
  Note: the benchmark does not contain the generated plots,
  the notebook needs to be executed by a Jupyter kernel first.

    %(prog)s notebook result.json -o my_notebook.ipynb
    jupyter nbconvert --to notebook --execute --inplace my_notebook

* Create a template of README.md used to enrich generated notebook

    %(prog)s readme result.json
    Edit README.md to replace/remove FIXMEs
    %(prog)s notebook -r README.md -o my_notebook.ipynb result.json
"""
import argparse
from collections import Mapping, namedtuple
import copy
import io
from pprint import pprint
import json
import sys

import humanize


class Cache(namedtuple("Cache", ["type", "level", "size", "num_sharing"])):
    @staticmethod
    def from_json(data):
        return Cache(**data)


class Context(
    namedtuple(
        "Context",
        [
            "date",
            "library_build_type",
            "executable",
            "mhz_per_cpu",
            "num_cpus",
            "cpu_scaling_enabled",
            "caches",
        ],
    )
):
    @staticmethod
    def from_json(data):
        caches = [Cache.from_json(cache) for cache in data.pop("caches")]
        return Context(caches=caches, **data)


class Line(namedtuple("Line", ["name", "values", "timings"])):
    """Plot line"""

    pass


class Benchmark(namedtuple("Benchmark", ["raw_title", "lines", "max_timing"])):
    """One benchmark made of several lines"""

    @property
    def title(self):
        """Benchmark title from C++ function name"""
        return self.raw_title.replace("_", " ").capitalize()

    @property
    def time_unit(self):
        """FIXME: adapt according to lines data"""
        return "ms"

    @property
    def xticks(self):
        ticks = set()
        for line in self.lines:
            for value in line.values:
                ticks.add(value)
        ticks = sorted(list(ticks))
        return ticks

    @property
    def xticklabels(self):
        return [humanize.naturalsize(xtick, gnu=True) for xtick in self.xticks]

    @staticmethod
    def from_json(title, benchmarks):
        """Create an instance of this class based on data in JSON file

        Args:
            title: first part of the C++ function name
            benchmarks(list of dict): benchmark data, one per line to plot
        """
        lines = dict()
        max_timing = 0
        for benchmark in benchmarks:
            name, value = benchmark["name"].split("__", 1)[1].split("/", 1)
            timing = benchmark["real_time"]
            if timing > max_timing:
                max_timing = timing
            time_unit = benchmark["time_unit"]
            if time_unit != "ns":
                raise Exception("Unexpected 'time_unit': " + time_unit)
            line = lines.setdefault(name, Line(name=name, values=[], timings=[]))
            line.values.append(int(value))
            line.timings.append(timing / 1e6)
        return Benchmark(raw_title=title, lines=lines.values(), max_timing=max_timing)

    def plot(self):
        """plot the Matplotlib figure"""
        self.fig()

    def fig(self):
        """build the Matplotlib figure"""
        import matplotlib.pyplot as plt

        fig, ax = plt.subplots(1, 1, figsize=(12, 6))
        ax.set_title(self.title)
        ax.set_xlabel("elements")
        ax.xscale = "log"
        ax.set_ylabel("time (" + self.time_unit + ")")
        ax.set_xticks(self.xticks)
        ax.set_xticklabels(self.xticklabels)
        for line in self.lines:
            ax.plot(line.values, line.timings, marker="o", label=line.name)
        ax.legend()
        return fig


def group_benchmarks(data):
    """
    Gather plot lines per benchmark

    Args:
        data: list of benchmark executions (plot lines)

    Returns:
        dict(name -> [dict(...)])
    """
    result = dict()
    for benchmark in data:
        title = benchmark["name"].split("__", 1)[0]
        result.setdefault(title, []).append(benchmark)
    return {
        title: Benchmark.from_json(title, benchmarks)
        for title, benchmarks in result.items()
    }


def load_from_data(data):
    """Load context and benchmarks datastructures based on raw JSON data"""
    context = Context.from_json(data["context"])
    benchmarks = group_benchmarks(data["benchmarks"])
    return context, benchmarks


# NOTEBOOK-STOP-HERE


def load_from_json(file):
    with open(file) as istr:
        return load_from_data(json.load(istr))


BENCHMARKS_MD = "## Benchmarks"
EMPTY_NOTEBOOK = {
    "metadata": {
        "kernelspec": {
            "display_name": "Python 3",
            "language": "python",
            "name": "python3",
        },
        "language_info": {
            "codemirror_mode": {"name": "ipython", "version": 3},
            "file_extension": ".py",
            "mimetype": "text/x-python",
            "name": "python",
            "nbconvert_exporter": "python",
            "pygments_lexer": "ipython3",
            "version": "3.6.4",
        },
    },
    "nbformat": 4,
    "nbformat_minor": 2,
    "cells": [],
}


def read_markdown(md_file, benchmarks):
    """
    Extract additional benchmark information from specified Markdown file

    Args:
        md_file: Markdown file
        benchmarks: benchmark names in JSON

    Returns:
         dict(
            context="markdown text",
            benchmarks={
                "name": {
                    'header': 'markdown text',
                    'footer': 'markdown text',
                },
                ...
            }
    """
    result = dict(benchmarks={})
    with open(md_file) as istr:
        content = istr.read()
    pos = content.find(BENCHMARKS_MD)
    if pos == -1:
        return result
    content = "\n" + content[pos + len(BENCHMARKS_MD) :].lstrip()
    parts = [part for part in content.split("\n### ") if part]
    for i, part in enumerate(parts):
        name, content = part.split("\n", 1)
        if name == "Context" and i == 0:
            result["context"] = content
            continue
        if name not in benchmarks:
            continue
        sections = part.split("#### ")
        for section in sections:
            sname, content = section.split("\n", 1)
            if sname in ["Header", "Footer"]:
                result["benchmarks"].setdefault(name, {})[sname] = content
    return result


def generate_notebook(json_file, output=None, md_file=None, **kwargs):
    """Create Jupyter notebook

    Args:
        json_file: input JSON file
        output: output file (default sys.stdout)
        md_file: optional Markdown file with additional information
    """
    with open(__file__) as istr:
        this_file = istr.read()
        pos = this_file.find("# NOTEBOOK-STOP-HERE")
        this_file = this_file[:pos]
    notebook = copy.copy(EMPTY_NOTEBOOK)
    notebook["cells"].append(
        dict(
            cell_type="code",
            metadata={},
            outputs=[],
            execution_count=None,
            source=this_file,
        )
    )
    with io.StringIO() as ostr:
        with open(json_file) as istr:
            pprint(json.load(istr), ostr)
        notebook["cells"].append(
            dict(
                cell_type="code",
                metadata={},
                outputs=[],
                execution_count=None,
                source="_, benchmarks = load_from_data(" + ostr.getvalue() + ")\n",
            )
        )
    context, benchmarks = load_from_json(json_file)
    if md_file:
        md_data = read_markdown(md_file, benchmarks)
    else:
        md_data = dict(benchmarks={}, context=None)

    if md_data["context"]:
        notebook["cells"].append(
            dict(
                cell_type="markdown",
                metadata={},
                source="## Context\n" + md_data["context"],
            )
        )
    for name, benchmark in benchmarks.items():
        header = md_data["benchmarks"].get(name, {}).get("Header") or ""
        footer = md_data["benchmarks"].get(name, {}).get("Footer") or ""
        notebook["cells"].append(
            dict(
                cell_type="markdown",
                metadata={},
                source="## " + benchmark.title + "\n" + header,
            )
        )
        notebook["cells"].append(
            dict(
                cell_type="code",
                metadata={},
                outputs=[],
                execution_count=None,
                source="benchmarks[" + repr(name) + "].plot()\n",
            )
        )
        if footer:
            notebook["cells"].append(
                dict(cell_type="markdown", metadata={}, source=footer)
            )
    if output is None:
        json.dump(notebook, sys.stdout, indent=4)
    else:
        with open(output, "w") as ostr:
            json.dump(notebook, ostr, indent=4)


def plot_benchmarks(json_file, **kwargs):
    """
    Create one PNG for every benchmark

    Args:
        json_file: JSON benchmark file
    """
    import matplotlib

    matplotlib.use("PS")
    context, benchmarks = load_from_json(json_file)
    for benchmark in benchmarks.values():
        benchmark.fig().savefig(benchmark.raw_title + ".png")


def generate_readme(json_file, append=False, output=None, **kwargs):
    """Create default Markdown file

    Args:
        json_file: JSON benchmark file
        append: append to file or overwrite, default overwrite
    """
    context, benchmarks = load_from_json(json_file)
    with open(output, "a" if append else "w") as ostr:
        print(BENCHMARKS_MD, file=ostr)
        print(file=ostr)
        print("### Context", file=ostr)
        print(file=ostr)
        print("Running", context.executable, "  ", file=ostr)
        print(
            "Run on ({} X {} MHz CPUs)".format(context.num_cpus, context.mhz_per_cpu),
            file=ostr,
        )
        print("CPU Caches:", file=ostr)
        for cache in context.caches:
            print(
                "* L{}".format(cache.level),
                cache.type,
                cache.size,
                "(x{})".format(cache.num_sharing),
                file=ostr,
            )
        print(file=ostr)
        for benchmark in benchmarks:
            print("###", benchmark, file=ostr)
            print(file=ostr)
            print("#### Header", file=ostr)
            print(file=ostr)
            print(
                "**FIXME**: content will be embedded in notebook before the plot",
                file=ostr,
            )
            print(file=ostr)
            print("#### Footer", file=ostr)
            print(file=ostr)
            print(
                "**FIXME**: content will be embedded in notebook after the plot",
                file=ostr,
            )
            print(file=ostr)


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Plot google benchmark results",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=sys.modules[__name__].__doc__,
    )
    subparsers = parser.add_subparsers()

    notebook = subparsers.add_parser("notebook", help="Generate Jupyter Notebook")
    notebook.add_argument("json_file", help="json file written by google benchmark")
    notebook.add_argument(
        "-o",
        "--output",
        help="Destination file [default to standard output]",
        default="README.md",
    )
    notebook.add_argument(
        "-r", "--readme", help="Markdown file to embed in the notebook", dest="md_file"
    )
    notebook.set_defaults(func=generate_notebook)

    plot = subparsers.add_parser("plot", help="Generate PNG images")
    plot.add_argument("json_file", help="json file written by google benchmark")
    plot.set_defaults(func=plot_benchmarks)

    readme = subparsers.add_parser("readme", help="Generate README.md template")
    readme.add_argument("-a", "--append", action="store_true")
    readme.add_argument("json_file", help="json file written by google benchmark")
    readme.add_argument(
        "-o", "--output", help="Output file [default= %(default)s", default="README.md"
    )
    readme.set_defaults(func=generate_readme)

    args = parser.parse_args(args=argv)
    args.func(**vars(args))


if __name__ == "__main__":
    main()
