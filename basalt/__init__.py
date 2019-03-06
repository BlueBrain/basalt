from enum import Enum

from ._basalt import *  # noqa
from ._basalt import Neuron, Synapse, Astrocyte, MicroDomain
from ._basalt import __version__, __rocksdb_version__  # noqa


__all__ = [
    "Graph",
    "VertexType",
    "PayloadHelper",
    "Vertices",
    "Edges",
    "Status",
    "Neuron",
    "Synapse",
    "Astrocyte",
    "MicroDomain",
]


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
