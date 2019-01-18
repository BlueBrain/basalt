from enum import Enum

from _basalt import *  # noqa
from _basalt import Neuron, Synapse, Astrocyte, MicroDomain
from _basalt import __version__, __rocksdb_version__  # noqa


class NodeType(Enum):
    NEURON = 1
    SYNAPSE = 2
    ASTROCYTE = 3
    MICRODOMAIN = 4
    SEGMENT = 5


class PayloadHelper:
    NODE_TYPE_TO_PAYLOAD_CLASS = {
        NodeType.NEURON: Neuron,
        NodeType.SYNAPSE: Synapse,
        NodeType.ASTROCYTE: Astrocyte,
        NodeType.MICRODOMAIN: MicroDomain,
    }

    @classmethod
    def deserialize(cls, type, payload):
        if isinstance(type, tuple):
            type = NodeType(type[0])
        if not isinstance(type, NodeType):
            type = NodeType(type)
        obj = cls.NODE_TYPE_TO_PAYLOAD_CLASS[type]()
        obj.deserialize(payload)
        return obj
