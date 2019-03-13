"""basalt-cli - CLI utility to deal with a basalt NGV graph

Usage:
  basalt-cli ngv import neuroglial [--max=<nb>] [--create-vertices] <h5-file> <basalt-path>
  basalt-cli ngv import synaptic [--max=<nb>] [--create-vertices] <h5-file> <basalt-path>
  basalt-cli ngv import gliovascular [--max=<nb>] [--create-vertices] <h5-connectivity> <h5-data> <basalt-path>
  basalt-cli ngv import microdomain [--max=<nb>] [--create-vertices] <h5-data> <basalt-path>
  basalt-cli -h | --help
  basalt-cli --version

Options
  --max=<nb>  Maximum number of items to import [default: -1].
  -h --help   Show this screen.
  --version   Show version.
"""
import json
import sys

from docopt import docopt

from . import __version__, ngv


def main(argv=None):
    args = docopt(__doc__, version='basalt ' + __version__, argv=argv)
    if args.get('ngv'):
        if args.get('neuroglial'):
            if args.get('import'):
                summary = ngv.import_neuroglial(
                    args["<h5-file>"],
                    args["<basalt-path>"],
                    max_=int(args.get("--max")),
                    create_vertices=args.get("--create-vertices"),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
        elif args.get("synaptic"):
            if args.get("import"):
                summary = ngv.import_synaptic(
                    args["<h5-file>"],
                    args["<basalt-path>"],
                    max_=int(args.get("--max")),
                    create_vertices=args.get("--create-vertices"),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
        elif args.get("gliovascular"):
            if args.get("import"):
                summary = ngv.import_gliovascular(
                    args["<h5-connectivity>"],
                    args["<h5-data>"],
                    args["<basalt-path>"],
                    max_=int(args.get("--max")),
                    create_vertices=args.get("--create-vertices"),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
        elif args.get("microdomain"):
            if args.get("import"):
                summary = ngv.import_microdomain(
                    args["<h5-data>"],
                    args["<basalt-path>"],
                    max_=int(args.get("--max")),
                    create_vertices=args.get("--create-vertices"),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
