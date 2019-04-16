"""
Process JSON file written by google benchmarks in various ways:

* create one PNG for every benchmark

    %(prog)s plot result.json

* create a Jupyter notebook with the benchmark plots.
  Note: the benchmark does not contain the generated plots,
  the notebook needs to be executed by a Jupyter kernel first.

    %(prog)s notebook result.json -o my_notebook.ipynb
    jupyter nbconvert --to notebook --execute --inplace my_notebook
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
    pass


class Benchmark(namedtuple("Benchmark", ["raw_title", "label", "lines", "max_timing"])):
    @property
    def title(self):
        return self.raw_title.replace("_", " ").capitalize()

    @property
    def xticklabels(self):
        return [humanize.naturalsize(xtick, gnu=True) for xtick in self.xticks]

    @property
    def time_unit(self):
        return "ms"

    @property
    def xticks(self):
        ticks = set()
        for line in self.lines:
            for value in line.values:
                ticks.add(value)
        ticks = sorted(list(ticks))
        return ticks

    @staticmethod
    def from_json(title, label, benchmarks):
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
        return Benchmark(
            raw_title=title, label=label, lines=lines.values(), max_timing=max_timing
        )

    def plot(self):
        self.fig()

    def fig(self):
        import matplotlib.pyplot as plt

        fig, ax = plt.subplots(1, 1, figsize=(12, 6))
        ax.set_title(self.title)
        ax.set_xlabel(self.label)
        ax.xscale = "log"
        ax.set_ylabel("time (" + self.time_unit + ")")
        ax.set_xticks(self.xticks)
        ax.set_xticklabels(self.xticklabels)
        for line in self.lines:
            ax.plot(line.values, line.timings, marker="o", label=line.name)
        ax.legend()
        return fig


def group_benchmarks(data, labels=None):
    result = dict()
    for benchmark in data:
        title = benchmark["name"].split("__", 1)[0]
        result.setdefault(title, []).append(benchmark)
    return {
        title: Benchmark.from_json(title, labels.get(title, ""), benchmarks)
        for title, benchmarks in result.items()
    }


class UniqueLabel(Mapping):
    def __init__(self, label):
        self.__label = label

    def __getitem__(self, e):
        return self.__label

    def __iter__(self):
        return iter([self.__label])

    def __len__(self):
        return 1


def load_from_data(data):
    context = Context.from_json(data["context"])
    labels = UniqueLabel("elements")
    benchmarks = group_benchmarks(data["benchmarks"], labels)
    return context, benchmarks


# NOTEBOOK-STOP-HERE


def generate_notebook(json_file, output=None, **kwargs):
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
    for name, benchmark in benchmarks.items():
        notebook["cells"].append(
            dict(
                cell_type="markdown", metadata={}, source="## " + benchmark.title + "\n"
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
    if output is None:
        json.dump(notebook, sys.stdout, indent=4)
    else:
        with open(output, "w") as ostr:
            json.dump(notebook, ostr, indent=4)


def plot_benchmarks(json_file, **kwargs):
    import matplotlib
    matplotlib.use("PS")
    context, benchmarks = load_from_json(json_file)
    for benchmark in benchmarks.values():
        benchmark.fig().savefig(benchmark.raw_title + ".png")


ACTIONS = dict(notebook=generate_notebook, plot=plot_benchmarks)


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
        "-o", "--output", help="Destination file [default to standard output]"
    )
    notebook.set_defaults(func=generate_notebook)

    plot = subparsers.add_parser("plot", help="Generate PNG images")
    plot.add_argument("json_file", help="json file written by google benchmark")
    plot.set_defaults(func=plot_benchmarks)

    args = parser.parse_args(args=argv)
    args.func(**vars(args))


def load_from_json(file):
    with open(file) as istr:
        return load_from_data(json.load(istr))


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


if __name__ == "__main__":
    main()
