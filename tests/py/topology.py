from collections import OrderedDict
from enum import Enum
import functools
import json
import os.path as osp
import tempfile
import shutil
import unittest

import numpy as np

from basalt.topology import directed, edge, Graph, vertex
from basalt.serialization import PickleSerialization


def tempdir(func):
    @functools.wraps(func)
    def _func(*args):
        path = tempfile.mkdtemp()
        try:
            func(*args, path)
        finally:
            shutil.rmtree(path)

    return _func


class Skills(Graph):
    class Vertex(Enum):
        PERSON = 1
        SKILL = 2
        CATEGORY = 3

    vertex("person", Vertex.PERSON, "pickle")
    vertex("skill", Vertex.SKILL, "pickle")
    vertex("category", Vertex.CATEGORY, serialization="pickle", plural="categories")

    edge(Vertex.PERSON, Vertex.SKILL)
    edge(Vertex.SKILL, Vertex.CATEGORY)
    edge(Vertex.CATEGORY, Vertex.CATEGORY)


class PLInfluence(Graph):
    directed(True)

    class Vertex(Enum):
        LICENSE = 1
        LANGUAGE = 2
        DEVELOPER = 3

    vertex("license", Vertex.LICENSE, serialization="pickle")
    vertex("language", Vertex.LANGUAGE, serialization="pickle")
    vertex("developer", Vertex.DEVELOPER, serialization="pickle")
    edge(Vertex.LANGUAGE, Vertex.LICENSE)
    edge(Vertex.LANGUAGE, Vertex.LANGUAGE, name="influenced", plural="influenced")
    edge(Vertex.LANGUAGE, Vertex.DEVELOPER, name="author")

    def import_json_data(self):
        file_fmt = osp.join(osp.dirname(__file__), "PLGraph_{}.json")
        LG_SUFFIX = "_(programming_language)"
        developers_ids = OrderedDict()
        license_ids = OrderedDict()
        language_ids = OrderedDict()
        links = {}
        vip_pl = {"Java": "JAVA", "C": "C", "C++": "CPP", "Haskell": "HASKELL"}

        # load language -> developer data
        # dbpedia SPARQL query (http://dbpedia.org/sparql):
        #   SELECT *
        #   WHERE { ?p a <http://dbpedia.org/ontology/ProgrammingLanguage> .
        #   ?p <http://dbpedia.org/ontology/developer> ?developer . }
        with open(file_fmt.format("developer")) as istr:
            data = json.load(istr)['results']['bindings']
        for entry in data:
            developer = entry['developer']['value'].rsplit('/', 1)[-1].replace('_', ' ')
            developer_id = developers_ids.setdefault(developer, len(developers_ids))
            del developer
            language = entry['p']['value'].rsplit('/', 1)[-1]
            if language.endswith(LG_SUFFIX):
                language = language[: -len(LG_SUFFIX)]
            language_id = language_ids.setdefault(language, len(language_ids))
            alias = vip_pl.get(language)
            if alias is not None:
                setattr(self, alias, language_id)
            del language
            links.setdefault("la_de", set()).add((language_id, developer_id))

        print(sorted(language_ids.keys()))

        # load language -> license data
        # dbpedia SPARQL query (http://dbpedia.org/sparql):
        #   SELECT *
        #   WHERE { ?p a <http://dbpedia.org/ontology/ProgrammingLanguage> .
        #   ?p <http://dbpedia.org/ontology/license> ?license . }
        with open(file_fmt.format("license")) as istr:
            data = json.load(istr)['results']['bindings']
        for entry in data:
            license = entry['license']['value'].rsplit('/', 1)[-1].replace('_', ' ')
            license_id = license_ids.setdefault(license, len(license_ids))
            del license
            language = entry['p']['value'].rsplit('/', 1)[-1]
            if language.endswith(LG_SUFFIX):
                language = language[: -len(LG_SUFFIX)]
            language_id = language_ids.setdefault(language, len(language_ids))
            alias = vip_pl.get(language)
            if alias is not None:
                setattr(self, alias, language_id)
            del language
            links.setdefault("la_li", set()).add((language_id, license_id))

        # load language -> influenced language data
        # dbpedia SPARQL query (http://dbpedia.org/sparql):
        #   SELECT *
        #   WHERE { ?p a <http://dbpedia.org/ontology/ProgrammingLanguage> .
        #   ?p <http://dbpedia.org/ontology/inspired> ?inspired . }
        with open(file_fmt.format("influence")) as istr:
            data = json.load(istr)['results']['bindings']
        for entry in data:
            language = entry['p']['value'].rsplit('/', 1)[-1]
            if language.endswith(LG_SUFFIX):
                language = language[: -len(LG_SUFFIX)]
            language_id = language_ids.setdefault(language, len(language_ids))
            alias = vip_pl.get(language)
            if alias is not None:
                setattr(self, alias, language_id)
            del language
            influenced = entry['influenced']['value'].rsplit('/', 1)[-1]
            if influenced.endswith(LG_SUFFIX):
                influenced = influenced[: -len(LG_SUFFIX)]
            influenced = influenced.replace('_', ' ')
            influenced_id = language_ids.setdefault(influenced, len(language_ids))
            del influenced
            links.setdefault("la_la", set()).add((language_id, influenced_id))

        # import vertices
        for vtype, data in [
            (self.Vertex.DEVELOPER, developers_ids),
            (self.Vertex.LANGUAGE, language_ids),
            (self.Vertex.LICENSE, license_ids),
        ]:
            self.vertices.add(
                np.full((len(data),), vtype.value, dtype=np.int32),
                np.fromiter(data.values(), dtype=np.int64),
                [PickleSerialization.serialize(k) for k in data.keys()],
            )

        # import edges
        for htype, ttype, data in [
            (self.Vertex.LANGUAGE, self.Vertex.LICENSE, links["la_li"]),
            (self.Vertex.LANGUAGE, self.Vertex.LANGUAGE, links["la_la"]),
            (self.Vertex.LANGUAGE, self.Vertex.DEVELOPER, links["la_de"]),
        ]:
            for hid, tid in data:
                self.edges.add((htype.value, hid), (ttype.value, tid))


class TestPLGraph(unittest.TestCase):
    @tempdir
    def test_pl_graph(self, path):
        g = PLInfluence.from_path(path)
        g.import_json_data()

        self.assertEqual(len(g.vertices), 961)
        self.assertEqual(len(g.languages), 649)
        self.assertEqual(len(g.developers), 232)
        self.assertEqual(len(g.licenses), 80)
        self.assertEqual(len(g.edges), 1314)

        c = g.languages[g.C]
        self.assertEqual(c.id, g.C)
        self.assertEqual(c.data, "C")
        authors = [dev.data for dev in c.authors]
        self.assertCountEqual(
            authors, ['ANSI C', 'Cornell University', 'Dennis Ritchie', 'ISO standard']
        )
        self.assertEqual(len(list(c.influenced)), 24)

        # languages directly inspired from Java
        java = g.languages[g.JAVA]
        self.assertEqual(len(list(java.influenced)), 18)

        # languages directly inspired from C++
        cpp = g.languages[g.CPP]
        self.assertEqual(len(list(cpp.influenced)), 11)

        # languages directly inspired from Haskell
        haskell = g.languages[g.HASKELL]
        self.assertEqual(len(list(haskell.influenced)), 32)

        # languages recursively inspired by C
        c_rec_influenced = set()
        work = [g.languages[g.C]]
        while len(work):
            pl = work.pop()
            for influenced in pl.influenced:
                if influenced not in c_rec_influenced:
                    c_rec_influenced.add(influenced)
                    work.append(influenced)
        self.assertEqual(len(c_rec_influenced), 134)


class TestSkillGraph(unittest.TestCase):
    @tempdir
    def test_skills_graph(self, path):
        g = Skills.from_path(path)

        alice = g.persons.add(0, "Alice")

        alice.type, alice.id, repr(alice.data)
        self.assertEqual(alice.type, Skills.Vertex.PERSON)
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
