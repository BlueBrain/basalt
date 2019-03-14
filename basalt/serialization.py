import pickle

import numpy as np


class PickleSerialization:
    @classmethod
    def serialize(cls, obj):
        bytes = pickle.dumps(obj)
        return np.ndarray((len(bytes),), dtype=np.byte, buffer=bytes)

    @classmethod
    def deserialize(cls, obj):
        return pickle.loads(np.ndarray.tostring(obj))


class BasaltPayloadSerialization:
    def __init__(self, payload_cls):
        self.payload_cls = payload_cls

    def serialize(self, data):
        return data.serialize()

    def deserialize(self, data):
        obj = self.payload_cls()
        obj.deserialize(data)
        return obj


class NoneSerialization:
    @classmethod
    def serialize(cls, obj):
        return obj

    @classmethod
    def deserialize(cls, data):
        return data


def serialization_method(name):
    if name == "pickle":
        return PickleSerialization
    elif getattr(name, "__module__", None) == "basalt._basalt.ngv":
        return BasaltPayloadSerialization(name)
    elif name is None:
        return NoneSerialization
    else:
        raise ValueError("Unexpected serialization method '" + name + "'")
