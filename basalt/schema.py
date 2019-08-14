"""
Copyright (C) 2019 Blue Brain Project

This file is part of Basalt distributed under the terms of the GNU
Lesser General Public License. See top-level LICENSE file for details.
"""

import collections
import functools
import sys

from cached_property import cached_property
import numpy as np
from six import string_types, with_metaclass

from basalt import Graph
from .serialization import serialization_method

__all__ = ["vertex", "edge", "MetaGraph"]


def dedupe(sequence):
    """Yields a stable de-duplication of an hashable sequence

    Args:
        sequence: hashable sequence to be de-duplicated

    Returns:
        stable de-duplication of the sequence
    """
    seen = set()
    for x in sequence:
        if x not in seen:
            yield x
            seen.add(x)


class DirectiveMeta(type):
    """Flushes the directives that were temporarily stored in the staging
    area into the package.
    """

    # Set of all known directives
    _directive_names = set()
    _directives_to_be_executed = []

    def __new__(cls, name, bases, attr_dict):
        # Initialize the attribute containing the list of directives
        # to be executed. Here we go reversed because we want to execute
        # commands:
        # 1. in the order they were defined
        # 2. following the MRO
        attr_dict["_directives_to_be_executed"] = []
        for base in reversed(bases):
            try:
                directive_from_base = base._directives_to_be_executed
                attr_dict["_directives_to_be_executed"].extend(directive_from_base)
            except AttributeError:
                # The base class didn't have the required attribute.
                # Continue searching
                pass

        # De-duplicates directives from base classes
        attr_dict["_directives_to_be_executed"] = [
            x for x in dedupe(attr_dict["_directives_to_be_executed"])
        ]

        # Move things to be executed from module scope (where they
        # are collected first) to class scope
        if DirectiveMeta._directives_to_be_executed:
            attr_dict["_directives_to_be_executed"].extend(
                DirectiveMeta._directives_to_be_executed
            )
            DirectiveMeta._directives_to_be_executed = []

        # Ignore any directives executed *within* top-level
        # directives by clearing out the queue they're appended to
        DirectiveMeta._directives_to_be_executed = []

        new_cls = super(DirectiveMeta, cls).__new__(cls, name, bases, attr_dict)
        if new_cls.__name__ != "MetaGraph":
            # Ensure the presence of the dictionaries associated
            # with the directives
            for d in DirectiveMeta._directive_names:
                setattr(new_cls, d, {})

            # Lazily execute directives
            for directive in new_cls._directives_to_be_executed:
                directive(new_cls)

            new_cls._generate_methods()
        return new_cls

    @staticmethod
    def directive(dicts=None):
        """Decorator for Basalt directives.

        Basalt directives allow you to describe a graph typology
        while defining a graph Python class., e.g. describe the kind
        of vertices and the possible edges between these vertices.

        Here's an example directive:

            @directive(dicts='edges_types')
            edge(name, ...):
                ...

        This directive allows you write:

            class Foo(MetaGraph):
                edge(...)

        The ``@directive`` decorator handles a couple things for you:

          1. Adds the class scope (graph) as an initial parameter when
             called, like a class method would.  This allows you to modify
             a package from within a directive, while the package is still
             being defined.

          2. It automatically adds a dictionary called "edges_types" to the
             graph class so that you can refer to it later..

        The ``(dicts='versions')`` part ensures that ALL packages in Spack
        will have a ``versions`` attribute after they're constructed, and
        that if no directive actually modified it, it will just be an
        empty dict.

        This is just a modular way to add storage attributes to the
        Package class, and it's how Spack gets information from the
        packages to the core.


        """
        global __all__

        if isinstance(dicts, string_types):
            dicts = (dicts,)
        if not isinstance(dicts, collections.Sequence):
            message = "dicts arg must be list, tuple, or string. Found {0}"
            raise TypeError(message.format(type(dicts)))
        # Add the dictionary names if not already there
        DirectiveMeta._directive_names |= set(dicts)

        # This decorator just returns the directive functions
        def _decorator(decorated_function):
            __all__.append(decorated_function.__name__)

            @functools.wraps(decorated_function)
            def _wrapper(*args, **kwargs):
                # If any of the arguments are executors returned by a
                # directive passed as an argument, don't execute them
                # lazily. Instead, let the called directive handle them.
                # This allows nested directive calls in packages.  The
                # caller can return the directive if it should be queued.
                def remove_directives(arg):
                    directives = DirectiveMeta._directives_to_be_executed
                    if isinstance(arg, (list, tuple)):
                        # Descend into args that are lists or tuples
                        for a in arg:
                            remove_directives(a)
                    else:
                        # Remove directives args from the exec queue
                        remove = next((d for d in directives if d is arg), None)
                        if remove is not None:
                            directives.remove(remove)

                # Nasty, but it's the best way I can think of to avoid
                # side effects if directive results are passed as args
                remove_directives(args)
                remove_directives(kwargs.values())

                # A directive returns either something that is callable on a
                # package or a sequence of them
                result = decorated_function(*args, **kwargs)

                # ...so if it is not a sequence make it so
                values = result
                if not isinstance(values, collections.Sequence):
                    values = (values,)

                DirectiveMeta._directives_to_be_executed.extend(values)

                # wrapped function returns same result as original so
                # that we can nest directives
                return result

            return _wrapper

        return _decorator


directive = DirectiveMeta.directive


@directive(dicts="vertex_types")
def vertex(name, type, serialization=None, plural=None, default_payload=True):
    """Declare a vertex type

    Args:
        name(str): vertex name
        type(enum): enum value
        serialization: it can take several values:

            * ``"pickle"``: whenever an object will be attached to a vertex
              of this type, the pickle module will be used to serialize/deserialize it.

            * ``None`` (default): payload specified when creating a vertex if passed
              as is to the low level graph, that only accepts
              ``numpy.ndarray(shape=(N,), dtype=numpy.byte)``

        plural(string): overwrite default plural (name + 's')
        default_payload(bool): whether new vertex has an empty payload (default: True)
    """

    def _register(metagraph):
        metagraph.vertex_types[name] = (type, serialization, plural, default_payload)

    return _register


@directive(dicts="edges_types")
def edge(lhs, rhs, serialization=None, default_payload=True):
    """Directive to declare an edge between 2 type of vertices

    Args:
        lhs(enum value): vertex type at one end of the edge
        rhs(enum value): vertex type at the other end of the edge
        serialization: optional serialization method. see :func:`vertex`
        default_payload(bool): whether new edge has an empty payload (default: True)
    """

    def _register(metagraph):
        metagraph.edges_types.setdefault(lhs, set()).add(
            (rhs, serialization, default_payload)
        )
        metagraph.edges_types.setdefault(rhs, set()).add(
            (lhs, serialization, default_payload)
        )

    return _register


class VerticesWrapper:
    """Wrapper class for all vertices of a certain type"""

    def __init__(self, g, vertex_info):
        """
        Args:
            g(:class:`basalt.graph`): instance of graph
            vertex_info(VertexInfo): details about the wrapped vertex
        """
        self.g = g
        self.type = vertex_info.type.value
        self._vertex_cls = vertex_info.cls

    def add(self, id, data=None, **kwargs):
        if data is None:
            data = self._vertex_cls.default_payload()
        if data is not None:
            data = self._vertex_cls.serialize(data)
            self.g.vertices.add((self.type, id), data, **kwargs)
        else:
            self.g.vertices.add((self.type, id), **kwargs)
        return self._vertex_cls(self.g, id)

    def get(self, id):
        """get deserialized payload of to the specified vertex

        Args:
            id(int): vertex identifier

        Returns:
            Deserialize object if any, `None` otherwise
        """
        data = self.g.vertices.get((self.type, id))
        if data is not None:
            data = self._vertex_cls.deserialize(data)
        return data

    def __getitem__(self, id):
        """get wrapper object representing the specified vertex

        Args:
            id(int): vertex identifier
        """
        data = self.g.vertices[(self.type, id)]
        return self._vertex_cls(self.g, id, data)

    def discard(self, id):
        """Remove the given vertex if present

        Args:
            id(int): vertex identifier
        """
        return self.g.vertices.discard((self.type, id))

    def __len__(self):
        """Count number of vertices

        Returns:
             Number of vertices of this type
        """
        return self.g.vertices.count(self.type)

    def __contains__(self, id):
        """Check presence of a given vertex

        Args:
            id(int): vertex identifier

        Returns:
            `True` if vertex is present, `False` otherwise
        """
        return (self.type, id) in self.g.vertices

    def __iter__(self):
        """Get iterator over vertices of this type
        """
        for vertex in self.g.vertices:
            if vertex[0] == self.type:
                yield vertex

    def clear(self):
        """Remove all vertices of this type"""
        raise NotImplementedError


class VertexInfo(
    collections.namedtuple("VertexInfo", ["name", "plural", "type", "connections"])
):
    """Hold information about a vertex type

    Attributes:
        name: vertex name
        plural: name to mention several of them
        type: vertex enum type
        connections: enum value of connected vertices
        cls: Wrapper class
    """

    @property
    def cls(self):
        return self.__cls

    @cls.setter
    def cls(self, cls):
        self.__cls = cls


class MetaGraph(with_metaclass(DirectiveMeta)):
    @classmethod
    def _generate_methods(cls):
        """Shape MetaGraph child class according to the :func:`vertex` and :func:`edge`
        directives.
        """
        cls._data_serializers = cls._create_data_serializers()
        cls._vertices = {
            type: VertexInfo(
                name,
                plural if plural else name + "s",
                type,
                (e[0] for e in cls.edges_types.get(type, [])),
            )
            for name, (type, _, plural, _) in cls.vertex_types.items()
        }

        for info in cls._vertices.values():
            info.cls = cls._create_vertex_class(info)
            cls._register_vertex_class(info.cls)
            cls._add_vertex_property(info)

    @classmethod
    def _add_vertex_property(cls, info):
        """Add a property at class top-level to access the wrapper of the
        specified vertex
        """

        def create_property():
            def getter(self):
                return VerticesWrapper(self.g, info)

            getter.__doc__ = "Manipulate vertices of type " + info.name
            return cached_property(getter)

        setattr(cls, info.plural, create_property())

    @classmethod
    def _register_vertex_class(cls, a_cls):
        """Register the given class in the module of the graph it is related"""
        cls_module = sys.modules[cls.__module__]
        # add vertex class to the module of graph class
        setattr(cls_module, a_cls.__name__, a_cls)
        # register class in __all__ of the module, if any
        if getattr(cls_module, "__all__", None):
            cls_module.__all__.append(a_cls.__name__)

    @classmethod
    def _create_data_serializers(cls):
        """Build a dict mapping every type of vertices and edges
        to the proper serialization methods. For example:

        {
        <VertexType.Foo: 3>:
            basalt.serialization.PickleSerialization,
        <VertexType.Bar: 2>:
            basalt.serialization.NoneSerialization,
        (<VertexType.Foo: 3>, <VertexType.Bar: 2>):
            basalt.serialization.PickleSerialization,
        (<VertexType.Bar: 2>, <VertexType.Foo: 3>):
            basalt.serialization.PickleSerialization
        }

        """
        eax = dict()
        for type, method, _, default_payload in cls.vertex_types.values():
            eax[type] = serialization_method(method, default_payload)
        for lhs, others in cls.edges_types.items():
            for rhs, method, default_payload in others:
                eax[(lhs, rhs)] = serialization_method(method, default_payload)
        return eax

    @classmethod
    def _create_vertex_class(cls, info):
        """Create a Vertex class dedicated to a certain vertex type

        Args:
            info(VertexInfo): details about the specified vertex type
        """

        class Vertex:
            def __init__(self, graph, id, data=None):
                assert id is not None
                assert isinstance(id, (np.integer, int))
                self._graph = graph
                self._id = id
                self._data = data
                if self._data is not None:
                    self._data = self.deserialize(self._data)

            @property
            def id(self):
                """get vertex identifier"""
                return self._id

            @property
            def type(self):
                """get vertex enum type"""
                return info.type

            @property
            def data(self):
                """get payload attached, if any

                Returns:
                    The deserialized payload is any, ``None`` otherwise
                """
                if self._data is None:
                    self._data = self._graph.vertices.get((info.type.value, self.id))
                    if self._data is not None:
                        self._data = self.deserialize(self._data)
                return self._data

            def update(self, **kwargs):
                """submit payload change

                Args:
                    kwargs: keyword arguments given to the graph instance beneath
                """
                self._graph.vertices.add(
                    (self.type.value, self.id), data=self.serialize(self.data), **kwargs
                )

            @classmethod
            def serialize(vcls, data):
                """Serialize given object

                Args:
                    data: object to serialize

                Returns:
                    `numpy.ndarray(shape=(N,), dtype=numpy.byte)` as expected by
                    low-level basalt graph API.
                """
                return cls._data_serializers[info.type].serialize(data)

            @classmethod
            def deserialize(vcls, data):
                """Deserialize given payload

                Args:
                    data(`numpy.ndarray(shape=(N,), dtype=numpy.byte)`): array
                    to deserialize

                Returns:
                    deserialized object
                """
                return cls._data_serializers[info.type].deserialize(data)

            @classmethod
            def default_payload(vcls):
                return cls._data_serializers[info.type].default_payload()

            def add(self, vertex, *args, **kwargs):
                """Connect a vertex to this one

                Args:
                    vertex(VertexWrapper): vertex object to connect

                Returns:
                    This instance
                """
                return self._add(vertex.type, vertex.id, *args, **kwargs)

            def discard(self, vertex):
                """Remove connection with given vertex

                Args:
                    vertex(VertexWrapper): vertex object to unlink
                """
                return self._discard(vertex.type, vertex.id)

            def _edges(self, type):
                for vertex_id in self._graph.edges.get(
                    (self.type.value, self.id), type.value
                ):
                    yield cls._vertices[type].cls(self._graph, vertex_id[1])

            def _add(self, type, id, data=None, **kwargs):
                if data is None:
                    data = cls._data_serializers[(self.type, type)].default_payload()
                if data is not None:
                    data = cls._data_serializers[(self.type, type)].serialize(data)
                    self._graph.edges.add(
                        (self.type.value, self.id),
                        (type.value, id),
                        data=data,
                        **kwargs
                    )
                else:
                    self._graph.edges.add(
                        (self.type.value, self.id), (type.value, id), **kwargs
                    )
                return self

            def _discard(self, type, id):
                self._graph.edges.discard(
                    ((self.type.value, self.id), (type.value, id))
                )
                return self

            def __getitem__(self, vertex):
                data = self._graph.edges.get(
                    ((self.type.value, self.id), (vertex.type.value, vertex.id))
                )
                if data is not None:
                    data = cls._data_serializers[(self.type, vertex.type)].deserialize(
                        data
                    )
                return data

        def _add(type_info):
            def _func(slf, id, *args, **kwargs):
                """Connect another {0}

                Args:
                    id(int): {0} identifier
                """
                return slf._add(type_info.type, id, *args, **kwargs)

            _func.__doc__ = _func.__doc__.format(type_info.name)
            return _func

        def _discard(type_info):
            def _func(slf, id, *args, **kwargs):
                """Disconnect another {0}

                Args:
                    id(int): {0} identifier
                """
                return slf._discard(type_info.type, id, *args, **kwargs)

            _func.__doc__ = _func.__doc__.format(type_info.name)
            return _func

        def _iterator(type_info):
            return property(
                fget=lambda self: self._edges(type_info.type),
                doc="Get iterable over connected " + type_info.name,
            )

        new_type = type(info.name.capitalize() + "Vertex", (Vertex, object), {})

        for vertex_type in info.connections:
            other_info = cls._vertices[vertex_type]
            setattr(new_type, other_info.plural, _iterator(other_info))
            setattr(new_type, "add_" + other_info.name, _add(other_info))
            setattr(new_type, "discard_" + other_info.name, _discard(other_info))
        return new_type

    @classmethod
    def from_path(cls, path, **kwargs):
        """Create a new graph instance

        Args:
            path(str): the path to basalt database on filesystem.
                The database is created if path does not exists

            kwargs: additional arguments given to the :class:`basalt.Graph` constructor
        """
        return cls(path=path, **kwargs)

    @classmethod
    def from_graph(cls, graph):
        """Create a new graph instance

        Args:
            graph(basalt.Graph): low-level graph instance
        """
        return cls(graph=graph)

    def __init__(self, graph=None, path=None, **kwargs):
        if graph is not None:
            self.g = graph
        else:
            assert path is not None
            self.g = Graph(path, **kwargs)

    @property
    def vertices(self):
        """See :func:`basalt.Graph.vertices`"""
        return self.g.vertices

    @property
    def edges(self):
        """See :func:`basalt.Graph.edges`"""
        return self.g.edges

    def commit(self):
        """See :func:`basalt.Graph.commit`"""
        return self.g.commit()

    def statistics(self):
        """See :func:`basalt.Graph.statistics`"""
        return self.g.statistics()
