import unittest

import numpy as np

import basalt


class Point(unittest.TestCase):
    def test_creation(self):
        p = basalt.Point()
        self.assertEqual(p.x, 0.0)
        self.assertEqual(p.y, 0.0)
        self.assertEqual(p.y, 0.0)

        p = basalt.Point(3.0, 4.0, 5.0)
        self.assertEqual(p.x, 3.0)
        self.assertEqual(p.y, 4.0)
        self.assertEqual(p.z, 5.0)

        p = basalt.Point([3.0, 4.0, 5.0])
        self.assertEqual(p.x, 3.0)
        self.assertEqual(p.y, 4.0)
        self.assertEqual(p.z, 5.0)

    def test_bracket_operators(self):
        p = basalt.Point(3.0, 4.0, 5.0)
        self.assertEqual(p[0], 3.0)
        self.assertEqual(p[1], 4.0)
        self.assertEqual(p[2], 5.0)

        p[1] = 6.0
        self.assertEqual(p[1], 6.0)

    def test_properties(self):
        p = basalt.Point(3.0, 4.0, 5.0)
        self.assertEqual(p.x, 3.0)
        self.assertEqual(p.y, 4.0)
        self.assertEqual(p.z, 5.0)

        p.y = 6.0
        self.assertEqual(p.y, 6.0)


class Neuron(unittest.TestCase):
    @staticmethod
    def create_object():
        n = basalt.Neuron()
        n.gid = 42
        n.astro_idx.append(43)
        n.astro_idx.append(44)
        n.astro_idx.append(45)
        n.syn_idx.append(46)
        n.syn_idx.append(47)
        n.syn_idx.append(48)
        return [n, basalt.Neuron(42, np.array([43, 44, 45]), np.array([46, 47, 48]))]

    def test_creation(self):
        n = basalt.Neuron(gid=42)
        self.assertEqual(list(n.astro_idx), [])
        self.assertEqual(list(n.syn_idx), [])

        n = basalt.Neuron(gid=42, syn_idx=[42])
        self.assertEqual(list(n.astro_idx), [])
        self.assertEqual(list(n.syn_idx), [42])

    def test_properties(self):
        neurons = Neuron.create_object()
        for n in neurons:
            self.assertEqual(n.gid, 42)
            self.assertEqual(list(n.astro_idx), [43, 44, 45])
            self.assertEqual(list(n.syn_idx), [46, 47, 48])

    def test_serialization(self):
        n, _ = Neuron.create_object()
        raw = n.serialize()
        n2 = basalt.Neuron()
        n2.deserialize(raw)
        self.check_object(n2)

    def check_object(self, obj):
        self.assertEqual(obj.gid, 42)
        self.assertEqual(list(obj.astro_idx), [43, 44, 45])
        self.assertEqual(list(obj.syn_idx), [46, 47, 48])


class Synapse(unittest.TestCase):
    @staticmethod
    def create_object():
        return basalt.Synapse(
            pre_gid=42,
            post_gid=43,
            nrn_idx=44,
            astro_idx=np.array([45, 46, 47]),
            is_excitatory=True,
            pre=basalt.Point(48.0, 49.0, 50.0),
            post=basalt.Point(51.0, 52.0, 53.0),
            mesh_filename="mesh_filename",
            skeleton_filename="skeleton_filename",
            psd_area=54.0,
        )

    def check_object(self, obj):
        self.assertEqual(obj.pre_gid, 42)
        self.assertEqual(obj.post_gid, 43)
        self.assertEqual(obj.nrn_idx, 44)
        self.assertEqual(list(obj.astro_idx), [45, 46, 47])
        self.assertTrue(obj.is_excitatory)
        self.assertEqual(list(obj.pre), [48.0, 49.0, 50.0])
        self.assertEqual(list(obj.post), [51.0, 52.0, 53.0])
        self.assertEqual(obj.mesh_filename, "mesh_filename")
        self.assertEqual(obj.skeleton_filename, "skeleton_filename")
        self.assertEqual(obj.psd_area, 54.0)

    def test_creation(self):
        s = basalt.Synapse()
        self.assertEqual(s.pre_gid, 0)

        s = Synapse.create_object()
        self.check_object(s)

    def test_serialization(self):
        obj = Synapse.create_object()
        raw = obj.serialize()

        obj = basalt.Synapse()
        obj.deserialize(raw)
        self.check_object(obj)


class MicroDomain(unittest.TestCase):
    @staticmethod
    def create_object():
        return basalt.MicroDomain(
            microdomain_id=0,
            astrocyte_id=1,
            neighbors=np.array([2, 3, 4]),
            vertex_coordinates=np.array([[5.0, 6.0, 7.0], [8.0, 9.0, 10.0]]),
            triangles=np.array([[11, 12, 13], [14, 15, 16]]),
            centroid=basalt.Point(17.0, 18.0, 19.0),
            area=20.0,
            volume=21.0,
            mesh_filename="mesh_filename",
            neurons_idx=np.array([22, 23, 24]),
            synapses_idx=np.array([25, 26, 27]),
        )

    def check_object(self, obj):
        self.assertEqual(obj.microdomain_id, 0)
        self.assertEqual(obj.astrocyte_id, 1)
        self.assertEqual(list(obj.neighbors), [2, 3, 4]),
        self.assertEqual(
            [list(vc) for vc in obj.vertex_coordinates],
            [[5.0, 6.0, 7.0], [8.0, 9.0, 10.0]],
        )
        self.assertEqual(list(obj.triangles), [[11, 12, 13], [14, 15, 16]])
        self.assertEqual(list(obj.centroid), [17.0, 18.0, 19.0])
        self.assertEqual(obj.area, 20.0)
        self.assertEqual(obj.volume, 21.0)
        self.assertEqual(obj.mesh_filename, "mesh_filename")
        self.assertEqual(list(obj.neurons_idx), [22, 23, 24])
        self.assertEqual(list(obj.synapses_idx), [25, 26, 27])

    def test_creation(self):
        m = basalt.MicroDomain()
        self.assertEqual(m.microdomain_id, 0)

        m = basalt.MicroDomain(microdomain_id=42)
        self.assertEqual(m.microdomain_id, 42)

        m = MicroDomain.create_object()
        raw = m.serialize()
        m = basalt.MicroDomain()
        m.deserialize(raw)
        self.check_object(m)


class Astrocyte(unittest.TestCase):
    @staticmethod
    def create_object():
        return basalt.Astrocyte(
            astrocyte_id=42,
            microdomain_id=43,
            soma_center=basalt.Point(44.0, 45.0, 46.0),
            soma_radius=47.0,
            name="name",
            mtype="mtype",
            morphology_filename="morphology_filename",
            synapses_idx=np.array([48, 49, 50]),
            neurons_idx=np.array([51, 52, 53]),
        )

    def check_object(self, a):
        self.assertEqual(a.astrocyte_id, 42)
        self.assertEqual(a.microdomain_id, 43)
        self.assertEqual(list(a.soma_center), [44.0, 45.0, 46.0])
        self.assertEqual(a.soma_radius, 47)
        self.assertEqual(a.name, "name")
        self.assertEqual(a.mtype, "mtype")
        self.assertEqual(a.morphology_filename, "morphology_filename")
        self.assertEqual(list(a.synapses_idx), [48, 49, 50])
        self.assertEqual(list(a.neurons_idx), [51, 52, 53])

    def test_creation(self):
        a = basalt.Astrocyte()
        self.assertEqual(a.astrocyte_id, 0)
        self.assertEqual(a.microdomain_id, 0)
        self.assertEqual(a.name, "")
        self.assertEqual(len(a.synapses_idx), 0)
        self.assertEqual(len(a.neurons_idx), 0)

        a = Astrocyte.create_object()
        self.check_object(a)

    def test_assign_properties(self):
        a = basalt.Astrocyte(astrocyte_id=42, name="astro")

        self.assertEqual(a.astrocyte_id, 42)
        a.astrocyte_id += 1
        self.assertEqual(a.astrocyte_id, 43)

        self.assertEqual(a.name, "astro")
        a.name = "gastro"
        self.assertEqual(a.name, "gastro")

    def test_serialization(self):
        a = self.create_object()
        raw = a.serialize()
        a2 = basalt.Astrocyte()
        a2.deserialize(raw)
        self.check_object(a2)


class Segment(unittest.TestCase):
    @staticmethod
    def create_object():
        return basalt.Segment(
            section_id=42,
            segment_id=43,
            type=44,
            x1=45.0,
            y1=46.0,
            z1=47.0,
            r1=48.0,
            x2=49.0,
            y2=50.0,
            z2=51.0,
            r2=52.0,
        )

    def check_object(self, obj):
        self.assertEqual(obj.section_id, 42)
        self.assertEqual(obj.segment_id, 43)
        self.assertEqual(obj.type, 44)
        self.assertEqual(obj.x1, 45.0)
        self.assertEqual(obj.y1, 46.0)
        self.assertEqual(obj.z1, 47.0)
        self.assertEqual(obj.r1, 48.0)
        self.assertEqual(obj.x2, 49.0)
        self.assertEqual(obj.y2, 50.0)
        self.assertEqual(obj.z2, 51.0)
        self.assertEqual(obj.r2, 52.0)

    def test_creation(self):
        s = basalt.Segment()
        self.assertEqual(s.section_id, 0)

        s = basalt.Segment(section_id=42)
        self.assertEqual(s.section_id, 42)
        self.assertEqual(s.segment_id, 0)

        s = Segment.create_object()
        self.check_object(s)

    def serialization(self):
        obj = Segment.create_object()
        raw = obj.serialize()
        obj = basalt.Segment()
        obj.deserialize(raw)
        self.check_object(obj)


class EdgeAstrocyteSegment(unittest.TestCase):
    @staticmethod
    def create_object():
        return basalt.EdgeAstrocyteSegment(
            astrocyte=basalt.Point(42.0, 43.0, 44.0),
            vasculature=basalt.Point(45.0, 46.0, 47.0),
        )

    def check_object(self, obj):
        self.assertEqual(list(obj.astrocyte), [42.0, 43.0, 44.0])
        self.assertEqual(list(obj.vasculature), [45.0, 46.0, 47.0])

    def test_creation(self):
        obj = basalt.EdgeAstrocyteSegment()
        self.assertEqual(list(obj.astrocyte), 3 * [0.0])
        self.assertEqual(list(obj.vasculature), 3 * [0.0])

        obj = EdgeAstrocyteSegment.create_object()
        self.check_object(obj)

    def test_serialization(self):
        obj = EdgeAstrocyteSegment.create_object()
        raw = obj.serialize()
        obj = basalt.EdgeAstrocyteSegment()
        obj.deserialize(raw)
        self.check_object(obj)


if __name__ == '__main__':
    unittest.main()
