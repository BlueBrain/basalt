"""basalt-cli - CLI utility to deal with a basalt connectivity graph

Usage:
  basalt-cli ngv import connectivity [--max-astrocytes=<nb>] [--create-nodes] <h5-file> <basalt-path>
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
        if args.get('connectivity'):
            if args.get('import'):
                summary = ngv.import_connectivity(
                    args['<h5-file>'],
                    args['<basalt-path>'],
                    create_nodes=args.get('--create-nodes'),
                    max_astrocytes=int(args.get('--max-astrocytes')),
                )
                json.dump(summary, sys.stdout, indent=2)
                sys.stdout.write("\n")
