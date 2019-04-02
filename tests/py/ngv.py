import functools
import os.path as osp
from packaging import version
import shutil
import tempfile
import unittest
from contextlib import contextmanager

import h5py
import numpy as np

import basalt.ngv as ngv


H5PY_HAS_FLOAT16 = version.parse(h5py.__version__) >= version.parse("2.8")


class TestH5Importer(unittest.TestCase):
    @property
    def data_dir(self):
        return osp.join(osp.dirname(__file__), "data")

    def h5_file(self, basename):
        return osp.join(self.data_dir, basename + ".h5")

    @contextmanager
    def tempdir(self):
        path = tempfile.mkdtemp()
        try:
            yield path
        finally:
            shutil.rmtree(path)

    def test_import_neuroglial(self):
        with self.tempdir() as path:
            file = self.h5_file("neuroglial_connectivity")
            stats = ngv.import_neuroglial(file, path, create_vertices=True)
            self.assertEqual(stats["dataset"], {"astrocytes": 4, "neurons": 20})

    @unittest.skipIf(not H5PY_HAS_FLOAT16, "This test requires h5py 2.8 or higher")
    def test_import_synaptic(self):
        with self.tempdir() as path:
            file = self.h5_file("synaptic_connectivity")
            stats = ngv.import_synaptic(file, path, create_vertices=True)
            self.assertEqual(stats["dataset"], {"synapses": 40, "neurons": 20})

    def test_import_gliovascular(self):
        with self.tempdir() as path:
            connectivity = self.h5_file("gliovascular_connectivity")
            data = self.h5_file("gliovascular_data")
            stats = ngv.import_gliovascular(
                connectivity, data, path, create_vertices=True
            )
            self.assertEqual(stats["dataset"], {"astrocytes": 4, "endfeet": 8})

    def test_import_microdomain(self):
        with self.tempdir() as path:
            data = self.h5_file("microdomain_structure")
            stats = ngv.import_microdomain(data, path, create_vertices=True)
            self.assertEqual(stats["dataset"], {"microdomains": 4})


def ngv_graph(func):
    @functools.wraps(func)
    def _func(*args):
        path = tempfile.mkdtemp()
        graph = ngv.NGVGraph(path=path)
        try:
            func(*args, graph)
        finally:
            del graph
            shutil.rmtree(path)

    return _func


class TestNGVGraph(unittest.TestCase):
    @ngv_graph
    def test_vertex_api(self, g):
        # add vertices
        g.astrocytes.add(1)
        g.astrocytes.add(2)

        # count
        self.assertEqual(len(g.vertices), 2)
        self.assertEqual(len(g.astrocytes), 2)

        # iterator
        vs = set()
        for v in g.astrocytes:
            vs.add(v)
        self.assertEqual(vs, set([(3, 1), (3, 2)]))

        # test existence
        self.assertIn(1, g.astrocytes)
        self.assertNotIn(3, g.astrocytes)

        # remove an unknown vertex
        g.astrocytes.discard(3)

        # remove a known vertex
        g.astrocytes.discard(2)
        self.assertEqual(len(g.astrocytes), 1)

    @ngv_graph
    def test_vertex_payload_api(self, g):
        astrocyte_payload = ngv.Astrocyte(
            soma_center=ngv.Point(44.0, 45.0, 46.0),
            soma_radius=47.0,
            name="name",
            mtype="mtype",
            morphology_filename="morphology_filename",
            synapses_idx=np.array([48, 49, 50]),
            neurons_idx=np.array([51, 52, 53]),
        )
        g.astrocytes.add(2, astrocyte_payload)
        a = g.astrocytes.get(2)
        self.assertEqual(list(a.soma_center), [44.0, 45.0, 46.0])
        self.assertEqual(a.soma_radius, 47)
        self.assertEqual(a.name, "name")
        self.assertEqual(a.mtype, "mtype")
        self.assertEqual(a.morphology_filename, "morphology_filename")
        self.assertEqual(list(a.synapses_idx), [48, 49, 50])
        self.assertEqual(list(a.neurons_idx), [51, 52, 53])

    @ngv_graph
    def test_edges_api(self, g):
        a1 = g.astrocytes.add(1)
        n2 = g.neurons.add(2)

        # create edge from vertex
        a1.add(n2)

        for neuron in a1.neurons:
            self.assertEqual(neuron.data, ngv.Neuron())
            self.assertEqual(neuron.id, 2)

        g.synapses.add(3)
        # create edge from identifier
        a1.add_synapse(3)

        for astrocyte in g.synapses[3].astrocytes:
            self.assertEqual(astrocyte.data, ngv.Astrocyte())
            self.assertEqual(astrocyte.id, 1)

        self.assertFalse(any(a1.discard_synapse(3).synapses))

    @ngv_graph
    def test_edges_payload_api(self, g):
        segment = g.segments.add(1)

        # connect a segment to an astrocyte, with an implicit payload
        astrocyte = g.astrocytes.add(1).add(segment)
        self.assertEqual(astrocyte[segment], ngv.EdgeAstrocyteSegment())

        # attach a payload on an edge between a segment and an astrocyte
        payload = ngv.EdgeAstrocyteSegment(
            astrocyte=ngv.Point(42.0, 43.0, 44.0),
            vasculature=ngv.Point(45.0, 46.0, 47.0),
        )
        astrocyte.add(segment, payload)

        self.assertEqual(astrocyte[segment], payload)
