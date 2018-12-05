import tempfile
import unittest

from basalt import make_id, Network, NodeType, Neuron, PayloadHelper


N42 = (NodeType.NEURON.value, 42)


class TestNetwork(unittest.TestCase):
    def test_persistence(self):
        path = tempfile.mkdtemp()

        g = Network(path)
        self.assertFalse(g.nodes.has(*N42))
        nid = g.nodes.insert(*N42)
        self.assertTrue(g.nodes.has(nid))
        del g

        g = Network(path)
        self.assertTrue(g.nodes.has(nid))

    def test_node_iteration(self):
        path = tempfile.mkdtemp()
        g = Network(path)
        self.assertEqual(list(g.nodes), [])

        uid = g.nodes.insert(*N42)
        count = 0
        for node in g.nodes:
            self.assertEqual(uid, node)
            count += 1
        self.assertEqual(count, 1)

    def test_deletion(self):
        path = tempfile.mkdtemp()

        g = Network(path)
        # try to erase a missing node from type and id
        g.nodes.erase(*N42).raise_on_error()
        nid = g.nodes.insert(*N42)
        self.assertTrue(g.nodes.has(nid))
        # erase node from uid
        g.nodes.erase(nid).raise_on_error()
        self.assertFalse(g.nodes.has(nid))

    def test_payload(self):
        path = tempfile.mkdtemp()
        g = Network(path)
        id = g.nodes.insert(NodeType.NEURON.value, 42, Neuron(gid=42).serialize())
        self.assertEqual(id, N42)
        del g

        g = Network(path)
        uid = make_id(NodeType.NEURON.value, 42)
        data = g.nodes.get(uid)
        self.assertIsNotNone(data)
        neuron = PayloadHelper.deserialize(uid, data)
        self.assertEqual(neuron.gid, 42)
        neuron.gid += 1
        self.assertEqual(neuron.gid, 43)
        neuron.astro_idx.append(43)
        neuron.syn_idx.append(44)
        neuron.syn_idx.append(45)
        g.nodes.insert(*uid, neuron.serialize(), commit=True)
        del g

        g = Network(path)
        uid = make_id(NodeType.NEURON.value, 42)
        data = g.nodes.get(uid)
        self.assertIsNotNone(data)
        neuron = PayloadHelper.deserialize(uid, data)
        self.assertEqual(neuron.gid, 43)
        self.assertEqual(list(neuron.astro_idx), [43])
        self.assertEqual(list(neuron.syn_idx), [44, 45])

    def test_single_connection(self):
        path = tempfile.mkdtemp()
        g = Network(path)
        A = make_id(NodeType.ASTROCYTE.value, 42)
        S1 = make_id(NodeType.ASTROCYTE.value, 1)

        # graph is empty, obviously these 2 nodes are not connected
        self.assertFalse(g.connections.has(A, S1))
        with self.assertRaises(RuntimeError):
            # cannot connect nodes that are not already in the graph
            g.connections.insert(A, S1)
        g.nodes.insert(*S1)
        with self.assertRaises(RuntimeError):
            # Node A is still not in the graph
            g.connections.insert(A, S1)
        g.nodes.insert(*A)
        # This time connection creation works
        g.connections.insert(A, S1)
        self.assertTrue(g.connections.has(A, S1))
        self.assertTrue(g.connections.has(S1, A))
        del g

        # test persistence
        g = Network(path)
        self.assertTrue(g.connections.has(A, S1))
        self.assertTrue(g.connections.has(S1, A))

        count = 0
        for node in g.connections.get(A):
            self.assertEqual(node, S1)
            count += 1
        self.assertEqual(count, 1)
        self.assertEqual(len(g.connections.get(A, NodeType.SYNAPSE.value)), 0)

        g.connections.erase(A, S1)
        self.assertFalse(g.connections.has(A, S1))
        self.assertFalse(g.connections.has(S1, A))


    def test_connections(self):
        path = tempfile.mkdtemp()
        g = Network(path)
        A = make_id(NodeType.ASTROCYTE.value, 42)
        S1 = make_id(NodeType.SEGMENT.value, 1)
        S2 = make_id(NodeType.SEGMENT.value, 2)
        Syn = make_id(NodeType.SYNAPSE.value, 1)

        g.nodes.insert(*A)
        g.nodes.insert(*S1)
        g.nodes.insert(*S2)
        g.nodes.insert(*Syn)
        g.connections.insert(A, S1)
        g.connections.insert(A, S2)
        g.connections.insert(A, Syn)
        g.commit()
        self.assertEqual(len(g.connections.get(A)), 3)
        self.assertEqual(len(g.connections.get(A, NodeType.SEGMENT.value)), 2)
        self.assertEqual(len(g.connections.get(A, NodeType.SYNAPSE.value)), 1)

    def test_node_removal(self):
        g = Network(tempfile.mkdtemp())
        A = make_id(0, 1)
        B = make_id(0, 2)
        C = make_id(0, 3)
        D = make_id(0, 4)
        g.nodes.insert(*A)
        g.nodes.insert(*B)
        g.nodes.insert(*C)
        g.nodes.insert(*D)
        g.connections.insert(A, B)
        g.connections.insert(B, C)
        g.connections.insert(C, D)
        g.connections.insert(D, A)
        self.assertTrue(g.nodes.has(A))
        self.assertTrue(g.nodes.has(B))
        self.assertTrue(g.nodes.has(C))
        self.assertTrue(g.nodes.has(D))
        self.assertTrue(g.connections.has(A, B))
        self.assertTrue(g.connections.has(B, C))
        self.assertTrue(g.connections.has(C, D))
        self.assertTrue(g.connections.has(D, A))
        self.assertTrue(g.connections.has(B, A))
        self.assertTrue(g.connections.has(C, B))
        self.assertTrue(g.connections.has(D, C))
        self.assertTrue(g.connections.has(A, D))
        self.assertCountEqual(list(g.connections.get(C)), [B, D])
        g.nodes.erase(A)
        self.assertFalse(g.nodes.has(A))
        self.assertEqual(g.connections.get(A), [])
        self.assertCountEqual(g.connections.get(B), [C])
        self.assertCountEqual(g.connections.get(C), [B, D])
        self.assertCountEqual(g.connections.get(D), [C])


class TestPayloadHelper(unittest.TestCase):
    def test_neuron(self):
        data = Neuron(gid=42).serialize()
        inputs = [
            NodeType.NEURON,
            NodeType.NEURON.value,
            make_id(NodeType.NEURON.value, 42),
        ]
        for input in inputs:
            neuron = PayloadHelper.deserialize(input, data)
            self.assertIsInstance(neuron, Neuron)
            self.assertEqual(neuron.gid, 42)


if __name__ == '__main__':
    unittest.main()
