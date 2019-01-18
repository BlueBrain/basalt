import datetime
import itertools
import os
import os.path as osp
import sys
import timeit

import humanize
import numpy as np
from progress.bar import ShadyBar

from . import ngv_connectome

from .. import Network, NodeType, make_id


CONNECTIVITY_H5 = 'neuroglial_connectivity.h5'


class AstrocytesArities:
    """Helper class to compute arities between
    astrocytes and its connected vertices.

    It is used to compute min, mean, max, and total edges between
    astrocytes <=> neurons and astrocytes <=> synapses
    """

    def __init__(self, labels):
        """
        :param labels: list of labels, one label representing a edge type
        connected to an astrocyte
        """
        self._labels = labels
        self._count = 0
        self._stats = np.array(len(labels) * [[-1, 0, 0]])

    def update(self, *counters):
        """Add arities of a particular astrocyte"""
        assert len(counters) == len(self._labels)
        self._count += 1
        """Update i-th label with new number of edges"""
        for i in range(len(counters)):
            count = counters[i]
            if self._stats[i][0] < 0 or count < self._stats[i][0]:
                self._stats[i][0] = count
            if count > self._stats[i][1]:
                self._stats[i][1] = count
            self._stats[i][2] += count

    def report(self):
        return dict(
            (
                label,
                dict(
                    minimum=int(self._stats[i][0]),
                    maximum=int(self._stats[i][1]),
                    mean=int(self._stats[i][2] / self._count),
                    total=int(self._stats[i][2]),
                ),
            )
            for i, label in enumerate(self._labels)
        )


def progress_if_tty(bar):
    if os.isatty(sys.stdout.fileno()):
        return bar

    class FakeBar:
        def next(self):
            pass

        def finish(self):
            pass

    return FakeBar()


def import_connectivity(h5_file, basalt_path, max_astrocytes=-1, create_nodes=False):
    _h5_file = osp.realpath(h5_file)
    if osp.isdir(_h5_file):
        _h5_file = osp.join(_h5_file, CONNECTIVITY_H5)
    graph = Network(basalt_path)

    with ngv_connectome.NeuroglialConnectivity(_h5_file) as connectivity:
        if max_astrocytes < 0:
            max_astrocytes = connectivity.n_astrocytes
        stats = AstrocytesArities(["synapses per astrocyte", "neurons per astrocyte"])

        bar = progress_if_tty(
            ShadyBar(
                'Importing data in basalt graph',
                max=max_astrocytes,
                suffix='%(index)d/%(max)d astrocytes',
            )
        )
        begin = timeit.default_timer()
        # astrocytes <--> synapses
        # astrocytes <--> neurons
        for astro_id in range(max_astrocytes):
            astro_node = make_id(NodeType.ASTROCYTE.value, astro_id)
            synapse_ids = connectivity.astrocyte.to_synapse(astro_id)
            graph.connections.insert(
                astro_node,
                NodeType.SYNAPSE.value,
                synapse_ids,
                create_nodes=create_nodes,
            )
            synapse_count = len(synapse_ids)
            del synapse_ids

            neuron_ids = connectivity.astrocyte.to_neuron(astro_id)
            graph.connections.insert(
                astro_node, NodeType.NEURON.value, neuron_ids, create_nodes=True
            )
            neuron_count = len(neuron_ids)
            stats.update(synapse_count, neuron_count)
            del neuron_ids
            bar.next()
        bar.finish()
        duration_sec = timeit.default_timer() - begin
        return dict(
            connectivity=stats.report(),
            duration_seconds=duration_sec,
            duration_str=humanize.naturaldelta(
                datetime.timedelta(seconds=duration_sec)
            ),
            h5_dataset=dict(
                astrocytes=connectivity.n_astrocytes, neurons=connectivity.n_neurons
            ),
            processed_astrocytes=max_astrocytes,
        )
