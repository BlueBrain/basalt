"""
Copyright (C) 2019 Blue Brain Project

This file is part of Basalt distributed under the terms of the GNU
Lesser General Public License. See top-level LICENSE file for details.
"""
# pylint: disable=no-name-in-module

import functools
from pkg_resources import get_distribution, DistributionNotFound

from ._basalt import Status, Vertices, Edges, Graph, make_id, default_config_file
from ._basalt import __rocksdb_version__  # noqa

try:
    __version__ = get_distribution(__name__).version
except DistributionNotFound:
    from ._basalt import __version__

__all__ = ["default_config_file", "Edges", "Graph", "make_id", "Status", "Vertices"]
