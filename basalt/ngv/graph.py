from enum import Enum

from basalt.schema import MetaGraph, edge, vertex

from . import Astrocyte, EdgeAstrocyteSegment, MicroDomain, Neuron, Segment, Synapse

__all__ = ["NGVGraph", "PayloadHelper", "VertexType"]


class VertexType(Enum):
    NEURON = 1
    SYNAPSE = 2
    ASTROCYTE = 3
    MICRODOMAIN = 4
    SEGMENT = 5


class PayloadHelper:
    VERTEX_TYPE_TO_PAYLOAD_CLASS = {
        VertexType.NEURON: Neuron,
        VertexType.SYNAPSE: Synapse,
        VertexType.ASTROCYTE: Astrocyte,
        VertexType.MICRODOMAIN: MicroDomain,
        VertexType.SEGMENT: Segment,
    }

    @classmethod
    def deserialize(cls, type, payload):
        if isinstance(type, tuple):
            type = VertexType(type[0])
        if not isinstance(type, VertexType):
            type = VertexType(type)
        obj = cls.VERTEX_TYPE_TO_PAYLOAD_CLASS[type]()
        obj.deserialize(payload)
        return obj


class NGVGraph(MetaGraph):
    vertex("astrocyte", VertexType.ASTROCYTE, Astrocyte)
    vertex("synapse", VertexType.SYNAPSE, Synapse)
    vertex("neuron", VertexType.NEURON, Neuron)
    vertex("microdomain", VertexType.MICRODOMAIN, MicroDomain)
    vertex("segment", VertexType.SEGMENT, Segment)

    edge(VertexType.ASTROCYTE, VertexType.SEGMENT, EdgeAstrocyteSegment)
    edge(VertexType.ASTROCYTE, VertexType.NEURON)
    edge(VertexType.ASTROCYTE, VertexType.SYNAPSE)
    edge(VertexType.ASTROCYTE, VertexType.MICRODOMAIN)
    edge(VertexType.MICRODOMAIN, VertexType.MICRODOMAIN)
    edge(VertexType.NEURON, VertexType.SYNAPSE)
