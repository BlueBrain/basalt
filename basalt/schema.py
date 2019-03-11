import collections
import functools
import pickle

from six import string_types, with_metaclass

from basalt import Graph

__all__ = ["vertex", "MetaGraph"]


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
        attr_dict['_directives_to_be_executed'] = []
        for base in reversed(bases):
            try:
                directive_from_base = base._directives_to_be_executed
                attr_dict['_directives_to_be_executed'].extend(directive_from_base)
            except AttributeError:
                # The base class didn't have the required attribute.
                # Continue searching
                pass

        # De-duplicates directives from base classes
        attr_dict['_directives_to_be_executed'] = [
            x for x in dedupe(attr_dict['_directives_to_be_executed'])
        ]

        # Move things to be executed from module scope (where they
        # are collected first) to class scope
        if DirectiveMeta._directives_to_be_executed:
            attr_dict['_directives_to_be_executed'].extend(
                DirectiveMeta._directives_to_be_executed
            )
            DirectiveMeta._directives_to_be_executed = []

        return super(DirectiveMeta, cls).__new__(cls, name, bases, attr_dict)

    def __init__(cls, name, bases, attr_dict):
        # The class is being created: if it is a package we must ensure
        # that the directives are called on the class to set it up
        if 'spack.pkg' in cls.__module__:
            # Package name as taken
            # from llnl.util.lang.get_calling_module_name
            pkg_name = cls.__module__.split('.')[-1]
            setattr(cls, 'name', pkg_name)

        # Ensure the presence of the dictionaries associated
        # with the directives
        for d in DirectiveMeta._directive_names:
            setattr(cls, d, {})

        # Lazily execute directives
        for directive in cls._directives_to_be_executed:
            directive(cls)

        # Ignore any directives executed *within* top-level
        # directives by clearing out the queue they're appended to
        DirectiveMeta._directives_to_be_executed = []

        super(DirectiveMeta, cls).__init__(name, bases, attr_dict)

    @staticmethod
    def directive(dicts=None):
        """Decorator for Spack directives.

        Spack directives allow you to modify a package while it is being
        defined, e.g. to add version or dependency information.  Directives
        are one of the key pieces of Spack's package "language", which is
        embedded in python.

        Here's an example directive:

            @directive(dicts='versions')
            version(pkg, ...):
                ...

        This directive allows you write:

            class Foo(Package):
                version(...)

        The ``@directive`` decorator handles a couple things for you:

          1. Adds the class scope (pkg) as an initial parameter when
             called, like a class method would.  This allows you to modify
             a package from within a directive, while the package is still
             being defined.

          2. It automatically adds a dictionary called "versions" to the
             package so that you can refer to pkg.versions.

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


@directive(dicts='vertex_types')
def vertex(name, type, serialization=None):
    def _register(metagraph):
        metagraph.vertex_types[name] = (type, serialization)

    return _register


@directive(dicts='edges_types')
def edge(lhs, rhs, serialization=None):
    def _register(metagraph):
        metagraph.edges_types.setdefault(lhs, set()).add((rhs, serialization))
        metagraph.edges_types.setdefault(rhs, set()).add((lhs, serialization))

    return _register


class VerticesWrapper:
    def __init__(self, g, type, vertex_cls):
        self.g = g
        self.type = type.value
        self._vertex_cls = vertex_cls

    def add(self, id, data=None, **kwargs):
        if data is not None:
            data = self._vertex_cls.serialize(data)
            self.g.vertices.add((self.type, id), data, **kwargs)
        else:
            self.g.vertices.add((self.type, id), **kwargs)
        return self._vertex_cls(id)

    def get(self, id):
        data = self.g.vertices.get((self.type, id))
        if data is not None:
            data = self._vertex_cls.deserialize(data)
        return data

    def __getitem__(self, id):
        data = self.g.vertices[(self.type, id)]
        return self._vertex_cls(id, data)

    def discard(self, id):
        return self.g.vertices.discard((self.type, id))

    def remove(self, id):
        return self.g.vertices.remove((self.type, id))

    def __len__(self):
        return self.g.vertices.count(self.type)

    def __contains__(self, id):
        return (self.type, id) in self.g.vertices

    def __iter__(self):
        for vertex in self.g.vertices:
            if vertex[0] == self.type:
                yield vertex

    def clear(self):
        raise NotImplementedError


def build_vertex_cls(
    name, type, graph, types_to_name, edges_types, vertices_cls, serializers
):
    class Vertex:
        def __init__(self, id, data=None):
            assert id is not None
            assert isinstance(id, int)
            self._id = id
            self._data = data
            if self._data is not None:
                self._data = self.deserialize(self._data)

        @property
        def id(self):
            return self._id

        @property
        def type(self):
            return type

        @property
        def data(self):
            if self._data is None:
                self._data = graph.vertices.get((type.value, self.id))
                if self._data is not None:
                    self._data = self.deserialize(self._data)
            return self._data

        @classmethod
        def serialize(cls, data):
            return serializers[type][0](data)

        @classmethod
        def deserialize(cls, data):
            return serializers[type][1](data)

        def connect(self, node, *args, **kwargs):
            return self._connect(node.type, node.id, *args, **kwargs)

        def _edges(self, type):
            for vertex_id in graph.edges.get((self.type.value, self.id), type.value):
                yield vertices_cls[type](vertex_id[1])

        def _connect(self, type, id, data=None, **kwargs):
            if data is not None:
                data = serializers[(self.type, type)][0](data)
                graph.edges.add(
                    (self.type.value, self.id), (type.value, id), data=data, **kwargs
                )
            else:
                graph.edges.add((self.type.value, self.id), (type.value, id), **kwargs)
            return self

        def _disconnect(self, type, id):
            graph.edges.discard(((self.type.value, self.id), (type.value, id)))
            return self

        def __getitem__(self, vertex):
            data = graph.edges.get(
                ((self.type.value, self.id), (vertex.type.value, vertex.id))
            )
            if data is not None:
                data = serializers[(self.type, vertex.type)][1](data)
            return data

    def _connect(type):
        @functools.wraps(Vertex._connect)
        def _func(slf, id, *args, **kwargs):
            return slf._connect(type, id, *args, **kwargs)

        return _func

    def _disconnect(type):
        @functools.wraps(Vertex._disconnect)
        def _func(slf, id, *args, **kwargs):
            return slf._disconnect(type, id, *args, **kwargs)

        return _func

    for edge_type in edges_types:
        edge_name = types_to_name[edge_type]
        setattr(Vertex, edge_name, property(lambda self: self._edges(edge_type)))
        setattr(Vertex, 'connect_' + edge_name, _connect(edge_type))
        setattr(Vertex, 'disconnect_' + edge_name, _disconnect(edge_type))
    return Vertex


class MetaGraph(with_metaclass(DirectiveMeta)):
    def __init__(self, *args, **kwargs):
        self.g = Graph(*args, **kwargs)
        vertices_cls = dict()

        def serialization_methods(serialization):
            if serialization == 'pickle':
                serialize = pickle.dumps
                deserialize = pickle.loads
            elif getattr(serialization, "__module__", None) == 'basalt._basalt.ngv':

                def _serialize(data):
                    return data.serialize()

                def _deserialize(data):
                    obj = serialization()
                    obj.deserialize(data)
                    return obj

                serialize = _serialize
                deserialize = _deserialize
            elif serialization is None:

                def identity(e):
                    return e

                serialize = identity
                deserialize = identity
            else:
                raise ValueError("Unexpected 'payload_cls' value")
            return serialize, deserialize

        serializers = dict()
        for type, method in self.vertex_types.values():
            serializers[type] = serialization_methods(method)
        for lhs, others in self.edges_types.items():
            for rhs, method in others:
                serializers[(lhs, rhs)] = serialization_methods(method)

        types_to_name = dict(
            (type, name) for name, (type, _) in self.vertex_types.items()
        )

        for name, (type, _) in self.vertex_types.items():
            vertex_cls = build_vertex_cls(
                name,
                type,
                self.g,
                types_to_name,
                [info[0] for info in self.edges_types.get(type, [])],
                vertices_cls,
                serializers,
            )
            vertices_cls[type] = vertex_cls

            setattr(self, name, VerticesWrapper(self.g, type, vertex_cls))

    @property
    def vertices(self):
        return self.g.vertices

    @property
    def edges(self):
        return self.g.edges

    def commit(self):
        return self.g.commit()

    def statistics(self):
        return self.g.statistics()
