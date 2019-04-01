from enum import Enum
import functools
import tempfile
import shutil
import unittest

import numpy as np

from basalt.schema import edge, MetaGraph, vertex


class Vertex(Enum):
    PERSON = 1
    SKILL = 2
    CATEGORY = 3


class Skills(MetaGraph):
    vertex("person", Vertex.PERSON, "pickle")
    vertex("skill", Vertex.SKILL, "pickle")
    vertex("category", Vertex.CATEGORY, serialization="pickle", plural="categories")

    edge(Vertex.PERSON, Vertex.SKILL)
    edge(Vertex.SKILL, Vertex.CATEGORY)
    edge(Vertex.CATEGORY, Vertex.CATEGORY)


def tempdir(func):
    @functools.wraps(func)
    def _func(*args):
        path = tempfile.mkdtemp()
        try:
            func(*args, path)
        finally:
            shutil.rmtree(path)

    return _func


class TestMetaGraph(unittest.TestCase):
    @tempdir
    def test_skills_graph(self, path):
        g = Skills.from_path(
            path
        )  # create instance from existing graph instance "graph"

        alice = g.persons.add(0, "Alice")

        alice.type, alice.id, repr(alice.data)
        self.assertEqual(alice.type, Vertex.PERSON)
        self.assertEqual(alice.id, 0)

        alice2 = g.persons[np.uint64(0)]
        self.assertEqual(alice.id, alice2.id)
        self.assertEqual(alice.type, alice2.type)
        self.assertEqual(alice.data, alice2.data)

        cpp = g.skills.add(42, "C++")

        # Java identifier 0 does not conflict with
        # Alice identifier because they have different types
        g.skills.add(0, "Java")

        alice.add(cpp)

        # or connect both from cpp identifier
        alice.add_skill(42)

        for skill in alice.skills:
            print(skill.id, skill.data)

        # remove edge
        alice.discard(cpp)

        pl = g.categories.add(0, "programming language")
        oo = g.categories.add(1, "object oriented")
        func = g.categories.add(2, "functional programming")

        pl.add_category(1)
        pl.add(func)

        cpp.add(oo)
        cpp.add_category(2)

        for category in cpp.categories:
            print(category)

        for category in g.categories:
            print(category)

        g.commit()
