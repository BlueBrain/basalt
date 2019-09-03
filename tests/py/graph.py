import json
import os
import os.path as osp
import tempfile
import unittest

import numpy as np

from basalt import UndirectedGraph, make_id, default_config_file

N42 = (0, 42)


class TestUndirectedGraph(unittest.TestCase):
    def test_persistence(self):
        path = tempfile.mkdtemp()

        g = UndirectedGraph(path)
        self.assertEqual(len(g.vertices), 0)
        self.assertFalse(N42 in g.vertices)
        g.vertices.add(N42)
        self.assertEqual(len(g.vertices), 1)
        self.assertTrue(N42 in g.vertices)
        del g

        g = UndirectedGraph(path)
        self.assertTrue(N42 in g.vertices)
        self.assertEqual(len(g.vertices), 1)

    def test_node_iteration(self):
        path = tempfile.mkdtemp()
        g = UndirectedGraph(path)
        self.assertEqual(list(g.vertices), [])

        g.vertices.add(N42)
        count = 0
        for node in g.vertices:
            self.assertEqual(N42, node)
            count += 1
        self.assertEqual(count, 1)

    def test_deletion(self):
        path = tempfile.mkdtemp()

        g = UndirectedGraph(path)
        # try to erase a missing node from type and id
        g.vertices.discard(N42)
        g.vertices.add(N42)
        self.assertTrue(N42 in g.vertices)
        # remove vertex
        g.vertices.discard(N42)
        self.assertTrue(N42 not in g.vertices)

    def test_single_connection(self):
        path = tempfile.mkdtemp()
        g = UndirectedGraph(path)
        A = make_id(1, 42)
        S1 = make_id(1, 1)

        # graph is empty, obviously these 2 vertices are not connected
        self.assertFalse((A, S1) in g.edges)
        with self.assertRaises(RuntimeError):
            # cannot connect vertices that are not already in the graph
            g.edges.add(A, S1)
        g.vertices.add(S1)
        with self.assertRaises(RuntimeError):
            # Node A is still not in the graph
            g.edges.add(A, S1)
        g.vertices.add(A)
        # This time connection creation works
        g.edges.add(A, S1)
        self.assertTrue((A, S1) in g.edges)
        self.assertTrue((S1, A) in g.edges)
        del g

        # test persistence
        g = UndirectedGraph(path)
        self.assertTrue((A, S1) in g.edges)
        self.assertTrue((S1, A) in g.edges)

        count = 0
        for node in g.edges.get(A):
            self.assertEqual(node, S1)
            count += 1
        self.assertEqual(count, 1)
        self.assertEqual(len(g.edges.get(A, 2)), 0)

        g.edges.discard((A, S1))
        self.assertNotIn((A, S1), g.edges)
        self.assertNotIn((S1, A), g.edges)

    def test_edges(self):
        path = tempfile.mkdtemp()
        g = UndirectedGraph(path)
        A = make_id(1, 42)
        S1 = make_id(3, 1)
        S2 = make_id(3, 2)
        Syn = make_id(2, 1)

        g.vertices.add(A)
        g.vertices.add(S1)
        g.vertices.add(S2)
        g.vertices.add(Syn)
        g.edges.add(A, S1)
        g.edges.add(A, S2)
        g.edges.add(A, Syn)
        g.commit()
        self.assertEqual(len(g.edges.get(A)), 3)
        self.assertEqual(len(g.edges.get(A, 3)), 2)
        self.assertEqual(len(g.edges.get(A, 2)), 1)

    def test_node_removal(self):
        g = UndirectedGraph(tempfile.mkdtemp())
        A = make_id(0, 1)
        B = make_id(0, 2)
        C = make_id(0, 3)
        D = make_id(0, 4)
        g.vertices.add(A)
        g.vertices.add(B)
        g.vertices.add(C)
        g.vertices.add(D)
        g.edges.add(A, B)
        g.edges.add(B, C)
        g.edges.add(C, D)
        g.edges.add(D, A)
        self.assertTrue(A in g.vertices)
        self.assertTrue(B in g.vertices)
        self.assertTrue(C in g.vertices)
        self.assertTrue(D in g.vertices)
        self.assertTrue((A, B) in g.edges)
        self.assertTrue((B, C) in g.edges)
        self.assertTrue((C, D) in g.edges)
        self.assertTrue((D, A) in g.edges)
        self.assertTrue((B, A) in g.edges)
        self.assertTrue((C, B) in g.edges)
        self.assertTrue((D, C) in g.edges)
        self.assertTrue((A, D) in g.edges)
        self.assertCountEqual(list(g.edges.get(C)), [B, D])
        g.vertices.discard(A)
        self.assertFalse(A in g.vertices)
        self.assertEqual(g.edges.get(A), [])
        self.assertCountEqual(g.edges.get(B), [C])
        self.assertCountEqual(g.edges.get(C), [B, D])
        self.assertCountEqual(g.edges.get(D), [C])

    def test_node_bulk_insertion_no_payload(self):
        g = UndirectedGraph(tempfile.mkdtemp())
        g.vertices.add(
            np.full(42, fill_value=3, dtype=np.int32), np.arange(42, dtype=np.uint64)
        )
        self.assertEqual(len(g.vertices), 42)


class TestConfig(unittest.TestCase):
    def test_default_config(self):
        fd, path = tempfile.mkstemp(suffix=".json")
        os.close(fd)
        os.remove(path)
        default_config_file(path)
        self.assertTrue(osp.exists(path))
        with open(path) as istr:
            config = json.load(istr)
        self.assertEqual(config["read_only"], False)
        self.assertEqual(config["statistics"], True)


if __name__ == '__main__':
    unittest.main()
