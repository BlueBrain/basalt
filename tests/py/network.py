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
