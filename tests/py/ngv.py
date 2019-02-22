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

    def test_import_connectivity(self):
        with self.tempdir() as path:
            file = self.h5_file("neuroglial_connectivity")
            stats = basalt.ngv.import_connectivity(file, path, create_nodes=True)
            self.assertEqual(stats["h5_dataset"], {"astrocytes": 4, "neurons": 20})
