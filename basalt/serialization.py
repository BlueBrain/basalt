"""
Copyright (C) 2019 Blue Brain Project

This file is part of Basalt distributed under the terms of the GNU
Lesser General Public License. See top-level LICENSE file for details.
"""

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

    @classmethod
    def default_payload(cls):
        return None


class BasaltPayloadSerialization:
    def __init__(self, payload_cls, default_payload):
        self.payload_cls = payload_cls
        self._default_payload = default_payload

    def serialize(self, data):
        return data.serialize()

    def deserialize(self, data):
        obj = self.payload_cls()
        obj.deserialize(data)
        return obj

    def default_payload(self):
        if self._default_payload:
            return self.payload_cls()
        return None


class NoneSerialization:
    @classmethod
    def serialize(cls, obj):
        return obj

    @classmethod
    def deserialize(cls, data):
        return data

    @classmethod
    def default_payload(cls):
        return None


def serialization_method(name, default_payload):
    if name == "pickle":
        return PickleSerialization
    elif hasattr(name, "serialize"):
        return BasaltPayloadSerialization(name, default_payload)
    elif name is None:
        return NoneSerialization
    else:
        raise ValueError("Unexpected serialization method '" + name + "'")
