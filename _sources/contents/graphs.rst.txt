Graphs
######

Definition
==========

Basalt allows manipulation of graphs. They can be directed or undirected.
A graph is made of vertices and edges.

A vertex is made of:
    * an Unique Identifier made of:
        * a type (``integer``)
        * an identifier (``integer``)
    * an optional payload

An edge is made of:
    * 2 vertices unique identifiers
    * an optional payload

A payload can be attached to vertices and edges
    Internally a payload is stored as
    a ``numpy.ndarray(dtype=numpy.byte)``, but it is possible to
    pass any object that supports pickle serialization


Basalt Python package provides 2 graph API to use undirected graphs:

* low level bindings of the basalt C++ library.
* a graph class generation API to generate Python classes dedicated
  to a particular graph topology.

Python bindings
===============

Basalt core is a C++ library that leverage rockdb to provide a graph oriented API.
This library is exposed in Python through bindings. The best way to get started right now
is to start reading reference documentation of :class:`basalt.UndirectedGraph`

Graph Class Generation
======================

Python bindings of C++ library are very low level, and is only about vertices and edges.
UndirectedGraph topology is not taken into account at all.

This Python API allows one to define a Python graph class from its topology.
For instance:

    >>> from enum import Enum
    >>> from basalt.topology import *
    >>>
    >>> class Vertex(Enum):
    ...     PERSON = 1
    ...     SKILL = 2
    ...     CATEGORY = 3
    >>>
    >>> class Skills(Graph):
    ...     vertex("person", Vertex.PERSON, "pickle")
    ...     vertex("skill", Vertex.SKILL, "pickle")
    ...     vertex("category", Vertex.CATEGORY, "pickle", "categories")
    ...
    ...     edge(Vertex.PERSON, Vertex.SKILL)
    ...     edge(Vertex.SKILL, Vertex.CATEGORY)
    ...     edge(Vertex.CATEGORY, Vertex.CATEGORY)


:mod:`basalt.topology` module provides 4 important symbols:

* :func:`basalt.topology.vertex`: directive to declare a vertex. It take a name,
  an enum value type, and optional a serialization method.
* :func:`basalt.topology.edge`: a directive to specify a connection between 2 type of
  vertices. Because an edge can also have a payload, a serialization method
  can also be given to the `edge` function.
* :func:`basalt.topology.directed`: a directive to specify whether the graph is directed or not.
  Default is undirected.
* :class:`basalt.topology.Graph`: a base class having a custom metaclass that takes
  the directives below into account.

The `Skills` class provides named methods to create vertices and edges so that it
is not necessary to pass vertices and edges types anymore.

It is possible to construct a ``Skills`` instance from either an existing graph instance
or from a filesystem path.

    >>> g = Skills(graph)  # create instance from existing graph instance "graph"
    >>>
    >>> alice = g.persons.add(0, "Alice")
    >>>
    >>> alice.type, alice.id, repr(alice.data)
    (<Vertex.PERSON: 1>, 0, "'Alice'")
    >>>
    >>> cpp = g.skills.add(42, "C++")
    >>>
    >>> # Java identifier 0 does not conflict with
    >>> # Alice identifier because they have different types
    >>> g.skills.add(0, "Java")
    <basalt.topology.SkillVertex...
    >>>
    >>> alice.add(cpp)
    <basalt.topology.PersonVertex...
    >>> # or connect both from cpp identifier
    >>> alice.add_skill(42)
    <basalt.topology.PersonVertex...
    >>>
    >>> for skill in alice.skills:
    ...   print(skill.id, skill.data)
    42 C++
    >>>
    >>> g.commit()
