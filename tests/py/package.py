import unittest

import basalt


class Version(unittest.TestCase):
    def test_rocksdb_version(self):
        self.assertIsTupleVersion(basalt.__rocksdb_version__)

    def test_basalt_version(self):
        self.assertIsTupleVersion(basalt.__version__)

    def assertIsTupleVersion(self, v):
        self.assertIsInstance(v, str)
        self.assertGreaterEqual(v.count('.'), 2)
