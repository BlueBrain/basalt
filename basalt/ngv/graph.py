from enum import Enum

from basalt.schema import MetaGraph, vertex

from . import Astrocyte, MicroDomain, Neuron, Synapse

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
    vertex("astrocytes", VertexType.ASTROCYTE, Astrocyte)
    vertex("synapses", VertexType.SYNAPSE, Synapse)
    vertex("neurons", VertexType.NEURON, Neuron)
    vertex("microdomains", VertexType.MICRODOMAIN, MicroDomain)
