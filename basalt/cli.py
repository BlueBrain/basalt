"""basalt-cli - CLI utility to deal with a basalt connectivity graph

Usage:
  basalt-cli ngv import neuroglial [--max-astrocytes=<nb>] [--create-nodes] <h5-file> <basalt-path>
  basalt-cli ngv import synaptic [--max-neurons=<nb>] [--create-nodes] <h5-file> <basalt-path>
  basalt-cli ngv import gliovascular [--max-astrocytes=<nb>] [--create-nodes] <h5-connectivity> <h5-data> <basalt-path>
  basalt-cli -h | --help
  basalt-cli --version

Options
  --max-astrocytes=<nb>  Maximum number of astrocytes to import [default: -1].
  -h --help              Show this screen.
  --version              Show version.
"""
import json
import sys

from docopt import docopt

from . import __version__
from . import ngv


def main(argv=None):
    args = docopt(__doc__, version='basalt ' + __version__, argv=argv)
    if args.get('ngv'):
        if args.get('neuroglial'):
            if args.get('import'):
                summary = ngv.import_neuroglial(
                    args['<h5-file>'],
                    args['<basalt-path>'],
                    max_astrocytes=int(args.get('--max-astrocytes')),
                    create_nodes=args.get('--create-nodes'),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
        elif args.get('synaptic'):
            if args.get('import'):
                summary = ngv.import_synaptic(
                    args['<h5-file>'],
                    args['<basalt-path>'],
                    max_neurons=int(args.get('--max-neurons')),
                    create_nodes=args.get('--create-nodes'),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
        elif args.get('gliovascular'):
            if args.get('import'):
                summary = ngv.import_gliovascular(
                    args['<h5-connectivity>'],
                    args['<h5-data>'],
                    args['<basalt-path>'],
                    max_astrocytes=int(args.get('--max-astrocytes')),
                    create_nodes=args.get('--create-nodes'),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
