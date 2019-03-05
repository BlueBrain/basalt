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

    def test_import_neuroglial(self):
        with self.tempdir() as path:
            file = self.h5_file("neuroglial_connectivity")
            stats = basalt.ngv.import_neuroglial(file, path, create_nodes=True)
            self.assertEqual(stats["dataset"], {"astrocytes": 4, "neurons": 20})

    def test_import_synaptic(self):
        with self.tempdir() as path:
            file = self.h5_file("synaptic_connectivity")
            stats = basalt.ngv.import_synaptic(file, path, create_nodes=True)
            self.assertEqual(stats["dataset"], {"synapses": 40, "neurons": 20})

    def test_import_gliovascular(self):
        with self.tempdir() as path:
            connectivity = self.h5_file("gliovascular_connectivity")
            data = self.h5_file("gliovascular_data")
            stats = basalt.ngv.import_gliovascular(
                connectivity, data, path, create_nodes=True
            )
            self.assertEqual(stats["dataset"], {"astrocytes": 4, "endfeet": 8})


    def test_import_microdomain(self):
        with self.tempdir() as path:
            data = self.h5_file("microdomain_structure")
            stats = basalt.ngv.import_microdomain(data, path, create_nodes=True)
            self.assertEquals(stats["dataset"], {"microdomains": 4})
