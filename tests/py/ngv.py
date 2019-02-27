from contextlib import contextmanager
import os.path as osp
import shutil
import tempfile
import unittest


import basalt.ngv


class TestImporter(unittest.TestCase):
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

    def test_import_neuroglial_connectivity(self):
        with self.tempdir() as path:
            file = self.h5_file("neuroglial_connectivity")
            stats = basalt.ngv.import_neuroglial_connectivity(
                file, path, create_nodes=True
            )
            self.assertEqual(stats["h5_dataset"], {"astrocytes": 4, "neurons": 20})

    def test_import_synaptic_connectivity(self):
        with self.tempdir() as path:
            file = self.h5_file("synaptic_connectivity")
            stats = basalt.ngv.import_synaptic_connectivity(
                file, path, create_nodes=True
            )
            self.assertEqual(stats["h5_dataset"], {"synapses": 40, "neurons": 20})

    def test_import_gliovascular(self):
        with self.tempdir() as path:
            connectivity = self.h5_file("gliovascular_connectivity")
            data = self.h5_file("gliovascular_data")
            stats = basalt.ngv.import_gliovascular(
                connectivity, data, path, create_nodes=True
            )
            self.assertEqual(stats["h5_dataset"], {"astrocytes": 4, "endfeet": 8})
