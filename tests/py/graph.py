import tempfile
import unittest

from basalt import Graph, make_id
from basalt.ngv import Neuron, PayloadHelper, VertexType

N42 = (VertexType.NEURON.value, 42)


class TestGraph(unittest.TestCase):
    def test_persistence(self):
        path = tempfile.mkdtemp()

        g = Graph(path)
        self.assertEqual(len(g.vertices), 0)
        self.assertFalse(N42 in g.vertices)
        g.vertices.add(N42)
        self.assertEqual(len(g.vertices), 1)
        self.assertTrue(N42 in g.vertices)
        del g

        g = Graph(path)
        self.assertTrue(N42 in g.vertices)
        self.assertEqual(len(g.vertices), 1)

    def test_node_iteration(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        self.assertEqual(list(g.vertices), [])

        g.vertices.add(N42)
        count = 0
        for node in g.vertices:
            self.assertEqual(N42, node)
            count += 1
        self.assertEqual(count, 1)

    def test_deletion(self):
        path = tempfile.mkdtemp()

        g = Graph(path)
        # try to erase a missing node from type and id
        g.vertices.discard(N42)
        g.vertices.add(N42)
        self.assertTrue(N42 in g.vertices)
        # remove vertex
        g.vertices.discard(N42)
        self.assertTrue(N42 not in g.vertices)

    def test_payload(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        uid = (VertexType.NEURON.value, 42)
        g.vertices.add(uid, Neuron(gid=42).serialize())
        del g

        g = Graph(path)
        data = g.vertices.get(uid)
        self.assertIsNotNone(data)
        neuron = PayloadHelper.deserialize(uid, data)
        self.assertEqual(neuron.gid, 42)
        neuron.gid += 1
        self.assertEqual(neuron.gid, 43)
        neuron.astro_idx.append(43)
        neuron.syn_idx.append(44)
        neuron.syn_idx.append(45)
        self.assertEqual(list(neuron.astro_idx), [43])
        self.assertEqual(list(neuron.syn_idx), [44, 45])
        g.vertices.add(uid, neuron.serialize(), commit=True)
        del g

        g = Graph(path)
        uid = make_id(VertexType.NEURON.value, 42)
        data = g.vertices.get(uid)
        self.assertIsNotNone(data)
        neuron = PayloadHelper.deserialize(uid, data)
        self.assertEqual(neuron.gid, 43)
        self.assertEqual(list(neuron.syn_idx), [44, 45])
        self.assertEqual(list(neuron.astro_idx), [43])

    def test_single_connection(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        A = make_id(VertexType.ASTROCYTE.value, 42)
        S1 = make_id(VertexType.ASTROCYTE.value, 1)

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
        g = Graph(path)
        self.assertTrue((A, S1) in g.edges)
        self.assertTrue((S1, A) in g.edges)

        count = 0
        for node in g.edges.get(A):
            self.assertEqual(node, S1)
            count += 1
        self.assertEqual(count, 1)
        self.assertEqual(len(g.edges.get(A, VertexType.SYNAPSE.value)), 0)

        g.edges.discard((A, S1))
        self.assertNotIn((A, S1), g.edges)
        self.assertNotIn((S1, A), g.edges)

    def test_edges(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        A = make_id(VertexType.ASTROCYTE.value, 42)
        S1 = make_id(VertexType.SEGMENT.value, 1)
        S2 = make_id(VertexType.SEGMENT.value, 2)
        Syn = make_id(VertexType.SYNAPSE.value, 1)

        g.vertices.add(A)
        g.vertices.add(S1)
        g.vertices.add(S2)
        g.vertices.add(Syn)
        g.edges.add(A, S1)
        g.edges.add(A, S2)
        g.edges.add(A, Syn)
        g.commit()
        self.assertEqual(len(g.edges.get(A)), 3)
        self.assertEqual(len(g.edges.get(A, VertexType.SEGMENT.value)), 2)
        self.assertEqual(len(g.edges.get(A, VertexType.SYNAPSE.value)), 1)

    def test_node_removal(self):
        g = Graph(tempfile.mkdtemp())
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


class TestPayloadHelper(unittest.TestCase):
    def test_neuron(self):
        data = Neuron(gid=42).serialize()
        inputs = [
            VertexType.NEURON,
            VertexType.NEURON.value,
            make_id(VertexType.NEURON.value, 42),
        ]
        for input in inputs:
            neuron = PayloadHelper.deserialize(input, data)
            self.assertIsInstance(neuron, Neuron)
            self.assertEqual(neuron.gid, 42)


if __name__ == '__main__':
    unittest.main()
