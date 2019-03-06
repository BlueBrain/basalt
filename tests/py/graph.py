import tempfile
import unittest

from basalt import make_id, Graph, VertexType, Neuron, PayloadHelper


N42 = (VertexType.NEURON.value, 42)


class TestGraph(unittest.TestCase):
    def test_persistence(self):
        path = tempfile.mkdtemp()

        g = Graph(path)
        self.assertFalse(g.vertices.has(*N42))
        nid = g.vertices.insert(*N42)
        self.assertTrue(g.vertices.has(nid))
        del g

        g = Graph(path)
        self.assertTrue(g.vertices.has(nid))

    def test_node_iteration(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        self.assertEqual(list(g.vertices), [])

        uid = g.vertices.insert(*N42)
        count = 0
        for node in g.vertices:
            self.assertEqual(uid, node)
            count += 1
        self.assertEqual(count, 1)

    def test_deletion(self):
        path = tempfile.mkdtemp()

        g = Graph(path)
        # try to erase a missing node from type and id
        g.vertices.erase(*N42).raise_on_error()
        nid = g.vertices.insert(*N42)
        self.assertTrue(g.vertices.has(nid))
        # erase node from uid
        g.vertices.erase(nid).raise_on_error()
        self.assertFalse(g.vertices.has(nid))

    def test_payload(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        id = g.vertices.insert(VertexType.NEURON.value, 42, Neuron(gid=42).serialize())
        self.assertEqual(id, N42)
        del g

        g = Graph(path)
        uid = make_id(VertexType.NEURON.value, 42)
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
        g.vertices.insert(*uid, neuron.serialize(), commit=True)
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
        self.assertFalse(g.edges.has(A, S1))
        with self.assertRaises(RuntimeError):
            # cannot connect vertices that are not already in the graph
            g.edges.insert(A, S1)
        g.vertices.insert(*S1)
        with self.assertRaises(RuntimeError):
            # Node A is still not in the graph
            g.edges.insert(A, S1)
        g.vertices.insert(*A)
        # This time connection creation works
        g.edges.insert(A, S1)
        self.assertTrue(g.edges.has(A, S1))
        self.assertTrue(g.edges.has(S1, A))
        del g

        # test persistence
        g = Graph(path)
        self.assertTrue(g.edges.has(A, S1))
        self.assertTrue(g.edges.has(S1, A))

        count = 0
        for node in g.edges.get(A):
            self.assertEqual(node, S1)
            count += 1
        self.assertEqual(count, 1)
        self.assertEqual(len(g.edges.get(A, VertexType.SYNAPSE.value)), 0)

        g.edges.erase(A, S1)
        self.assertFalse(g.edges.has(A, S1))
        self.assertFalse(g.edges.has(S1, A))

    def test_edges(self):
        path = tempfile.mkdtemp()
        g = Graph(path)
        A = make_id(VertexType.ASTROCYTE.value, 42)
        S1 = make_id(VertexType.SEGMENT.value, 1)
        S2 = make_id(VertexType.SEGMENT.value, 2)
        Syn = make_id(VertexType.SYNAPSE.value, 1)

        g.vertices.insert(*A)
        g.vertices.insert(*S1)
        g.vertices.insert(*S2)
        g.vertices.insert(*Syn)
        g.edges.insert(A, S1)
        g.edges.insert(A, S2)
        g.edges.insert(A, Syn)
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
        g.vertices.insert(*A)
        g.vertices.insert(*B)
        g.vertices.insert(*C)
        g.vertices.insert(*D)
        g.edges.insert(A, B)
        g.edges.insert(B, C)
        g.edges.insert(C, D)
        g.edges.insert(D, A)
        self.assertTrue(g.vertices.has(A))
        self.assertTrue(g.vertices.has(B))
        self.assertTrue(g.vertices.has(C))
        self.assertTrue(g.vertices.has(D))
        self.assertTrue(g.edges.has(A, B))
        self.assertTrue(g.edges.has(B, C))
        self.assertTrue(g.edges.has(C, D))
        self.assertTrue(g.edges.has(D, A))
        self.assertTrue(g.edges.has(B, A))
        self.assertTrue(g.edges.has(C, B))
        self.assertTrue(g.edges.has(D, C))
        self.assertTrue(g.edges.has(A, D))
        self.assertCountEqual(list(g.edges.get(C)), [B, D])
        g.vertices.erase(A)
        self.assertFalse(g.vertices.has(A))
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
