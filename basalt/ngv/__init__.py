import datetime
import inspect
import os
import os.path as osp
import sys
import timeit

import humanize
import numpy as np
from progress.bar import ShadyBar

from . import ngv_data
from .ngv_connectome import GliovascularConnectivity
from .ngv_connectome import NeuroglialConnectivity
from .ngv_connectome import SynapticConnectivity
from .. import Network, NodeType, make_id, EdgeAstrocyteSegment, MicroDomain, Point


CONNECTIVITY_H5 = 'neuroglial_connectivity.h5'


class NodesArities:
    """Helper class to compute arities between connected nodes

    It computes min, mean, max, and total edges between nodes during import of
    connectivity HDF5 files.
    """

    def __init__(self, labels):
        """
        :param labels: list of labels, one label representing a edge type
        connected to a particular node type
        """
        self._labels = labels
        self._count = 0
        self._stats = np.array(len(labels) * [[-1, 0, 0]])

    def update(self, *counters):
        """Add arities of a particular node type"""
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


def _resolve_path(h5_file):
    _h5_file = osp.realpath(h5_file)
    if osp.isdir(_h5_file):
        _h5_file = osp.join(_h5_file, CONNECTIVITY_H5)
    return _h5_file


def endfoot_id_to_payload(gliovascular, endfoot_id):
    """Create payload of the edgepayload from HDF5 file for a given astrocyte

    Args:
        gliovascular(ngv_data.GliovascularData): HDF5 file wrapper
        endfoot_id(int): endfoot identifier

    Returns:
        EdgeAstrocyteSegment: Basalt payload

    """
    return EdgeAstrocyteSegment(
        astrocyte=Point(gliovascular.endfoot_graph_coordinates[endfoot_id]),
        vasculature=Point(gliovascular.endfoot_surface_coordinates[endfoot_id]),
    )


def astro_id_to_microdomain(microdomain, astro_id):
    """Create microdomain payload from HDF5 file for a given astrocyte

    Args:
        microdomain(ngv_data.MicrodomainTesselation): HDF5 file wrapper
        astro_id(int): astrocyte identifier

    Returns:
        MicroDomain: Basalt payload

    """
    return MicroDomain(
        triangles=microdomain.domain_triangles(astro_id),
        vertex_coordinates=microdomain.domain_points(astro_id),
    )


class Importer:
    def __init__(self, title, connectivity, arity_labels, progress_kwargs):
        self._arities = NodesArities(arity_labels)
        self._begin = None
        self._end = None
        self._bar = progress_if_tty(ShadyBar(title, **progress_kwargs))
        self.iterations = 0
        self.connectivity = connectivity

    @property
    def dataset(self):
        schema = dict()
        for name, prop in inspect.getmembers(
            self.connectivity.__class__, lambda o: isinstance(o, property)
        ):
            if name.startswith('n_'):
                schema[name[2:]] = prop.fget(self.connectivity)
        return schema

    @property
    def duration_sec(self):
        return self._end - self._begin

    def next(self, *args):
        self._arities.update(*args)
        self._bar.next()
        self.iterations += 1

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._bar.finish()
        self._end = timeit.default_timer()

    def report(self):
        return dict(
            connectivity=self._arities.report(),
            dataset=self.dataset,
            duration_seconds=self.duration_sec,
            duration_str=humanize.naturaldelta(
                datetime.timedelta(seconds=self.duration_sec)
            ),
            iterations=self.iterations,
        )

    def __enter__(self):
        self._begin = timeit.default_timer()
        return self


def import_gliovascular(
    h5_connectivity, h5_data, basalt_path, max_=-1, create_nodes=False
):
    _h5_connectivity = _resolve_path(h5_connectivity)
    _h5_data = _resolve_path(h5_data)
    graph = Network(basalt_path)

    with GliovascularConnectivity(
        _h5_connectivity
    ) as connectivity, ngv_data.GliovascularData(_h5_data) as data:
        if max_ < 0:
            max_ = connectivity.n_astrocytes
        with Importer(
            title="Import astrocyte ⇆ vasculature segments",
            connectivity=connectivity,
            arity_labels=["segments per astrocyte"],
            progress_kwargs=dict(
                max=max_, suffix='%(index)d/%(max)d astrocytes'
            ),
        ) as importer:
            for astro_id in range(max_):
                astro_node = make_id(NodeType.ASTROCYTE.value, astro_id)
                begin, end = connectivity.astrocyte._offset_slice(astro_id, None)
                conn_data = connectivity.astrocyte._connectivity[begin:end]
                payloads = [
                    endfoot_id_to_payload(data, endfoot).serialize()
                    for endfoot in conn_data[:, 0]
                ]
                graph.connections.insert(
                    astro_node,
                    NodeType.SEGMENT.value,
                    conn_data[:, 1],
                    payloads,
                    create_nodes=create_nodes,
                )
                importer.next(len(conn_data))
        return importer.report()


def import_synaptic(h5_file, basalt_path, max_=-1, create_nodes=False):
    _h5_file = _resolve_path(h5_file)
    graph = Network(basalt_path)

    with SynapticConnectivity(_h5_file) as connectivity:
        if max_ < 0:
            max_ = connectivity.n_neurons
        with Importer(
            title="Importing synapse ⇆ afferent neuron",
            connectivity=connectivity,
            arity_labels=["synapse per neuron"],
            progress_kwargs=dict(max=max_, suffix='%(index)d/%(max)d neurons'),
        ) as importer:
            for neuron_id in range(max_):
                neuron_node = make_id(NodeType.NEURON.value, neuron_id)
                synapse_ids = connectivity.afferent_neuron.to_synapse(neuron_id)
                graph.connections.insert(
                    neuron_node,
                    NodeType.SYNAPSE.value,
                    synapse_ids,
                    create_nodes=create_nodes,
                )
                importer.next(len(synapse_ids))
        return importer.report()


def import_neuroglial(h5_file, basalt_path, max_=-1, create_nodes=False):
    _h5_file = _resolve_path(h5_file)
    graph = Network(basalt_path)

    with NeuroglialConnectivity(_h5_file) as connectivity:
        if max_ < 0:
            max_ = connectivity.n_astrocytes
        with Importer(
            title="Importing astrocyte ⇆ (synapses, neurons)",
            connectivity=connectivity,
            arity_labels=["synapses per astrocyte", "neurons per astrocyte"],
            progress_kwargs=dict(
                max=max_, suffix='%(index)d/%(max)d astrocytes'
            ),
        ) as importer:
            for astro_id in range(max_):
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
                del neuron_ids
                importer.next(synapse_count, neuron_count)
        return importer.report()


def import_microdomain(h5_file, basalt_path, max_=-1, create_nodes=False):
    """Import microdomain structure in HDF5 file in a basalt graph

    Import microdomains subgraph and attach every microdomain to its
    associated astrocyte.

    Args:
        h5_file: path to HDF5 file
        basalt_path: path to basalt graph
        max_: limit number of microdomains to import. If negative,
            then microdomains of every astrocytes are imported.
        create_nodes: if True then also creates astrocyte vertices
            connected to the microdomains

    Returns:
        A dict providing import information and statistics

    """
    _h5_file = _resolve_path(h5_file)
    graph = Network(basalt_path)

    with ngv_data.MicrodomainTesselation(_h5_file) as microdomain:
        if max_ < 0:
            max_ = microdomain.n_microdomains
        with Importer(
            title="Importing microdomain subgraph",
            connectivity=microdomain,
            arity_labels=["microdomains per microdomain"],
            progress_kwargs=dict(
                max=max_, suffix="%(index)d/%(max)d microdomains"
            ),
        ) as importer:
            micro_domain_edges = []
            micro_domain_payloads = []
            for astro_id in range(max_):
                # append microdomains connected to the `astro_id` one
                micro_domain_edges.append(microdomain.domain_neighbors(astro_id))
                # create microdomain payload and serialize it
                micro_domain_payloads.append(
                    astro_id_to_microdomain(microdomain, astro_id).serialize()
                )
            # insert all microdomain vertices all at once
            graph.nodes.insert(
                np.full(max_, NodeType.MICRODOMAIN.value, dtype=np.int32),
                np.arange(max_, dtype=np.uint64),
                micro_domain_payloads,
            )
            if create_nodes:
                # insert astrocyte vertices as well
                graph.nodes.insert(
                    np.full(max_, NodeType.ASTROCYTE.value, dtype=np.int32),
                    np.arange(max_, dtype=np.uint64),
                    [],
                )
            for astro_id, edges in enumerate(micro_domain_edges):
                # insert microdomains inter connectivity
                graph.connections.insert(
                    make_id(NodeType.MICRODOMAIN.value, astro_id),
                    NodeType.MICRODOMAIN.value,
                    edges,
                )
                # insert microdomain ⇆ astrocyte connection
                graph.connections.insert(
                    make_id(NodeType.MICRODOMAIN.value, astro_id),
                    make_id(NodeType.ASTROCYTE.value, astro_id),
                )
                importer.next(len(edges))
        return importer.report()
