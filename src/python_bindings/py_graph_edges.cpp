/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <vector>

#include <pybind11/stl.h>

#include "basalt/edge_iterator.hpp"
#include "basalt/edges.hpp"
#include "py_graph_edges.hpp"
#include "py_helpers.hpp"

namespace py = pybind11;
using pybind11::literals::operator""_a;

namespace basalt {

namespace docstring {

static const char* edges_class = R"(
    Manage edges of the graph

    An edge is made of 2 distinct vertices

    Optionally, a byte-array can be attached to an edge.

    Both vertices must exist prior creation of an edge:

    >>> graph.vertices.clear()
    >>> v1, v2 = [(0, 1), (0, 2)]
    >>> graph.vertices.add(v1)
    >>> graph.edges.add(v1, v2)
    Traceback (most recent call last):
      ...
    RuntimeError: Missing vertex (0:2)

    Both ends of an edge must exist first:

    >>> graph.vertices.clear()
    >>> graph.vertices.add(v1)
    >>> graph.vertices.add(v2)
    >>> graph.edges.add(v1, v2)
    >>> len(graph.edges)
    1

    if the graph is undirected, then the 2 possible
    uids represents the same edge:

    >>> (v1, v2) in graph.edges
    True
    >>> (v2, v1) in graph.edges
    True

    It is possible to iterate over the edges of the graph.
    Note that every edge appears in both direction:

    >>> for edge in sorted(graph.edges):
    ...   print(edge)
    ((0, 1), (0, 2))
    ((0, 2), (0, 1))

)";

static const char* discard_edge = R"(
    Remove edge between 2 vertices

    Args:
        edge(tuple): edge unique identifier to remove.
        commit(bool): whether uncommitted operations should be flushed or not.

    >>> v1, v2 = (0, 1), (0, 2)
    >>> graph.edges.discard((v1, v2))

)";

static const char* discard_edges = R"(
    Remove edges connected to a given vertex

    Args:
        vertex(tuple): vertex unique identifier.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

static const char* discard_edges_if = R"(
    Remove edges starting from a given vertex and where the vertex on the
    other end is of a certain type

    Args:
        vertex(tuple): vertex unique identifier from where the edge starts.
        type(int): type of the other end of the edge.
        commit(bool): whether uncommitted operations should be flushed or not

)";

static const char* get_data = R"(
    Get payload associated to an edge

    Args:
        edge(tuple): edge unique identifier.

    Returns:
        `None` is edge does not exist or does not have an associated payload.
        a `np.array(dtype=np.byte)` array otherwise.

    Raises:
        KeyError: if edge does not exist

    >>> graph.vertices.clear()
    >>> v1, v2 = [(0, 1), (0, 2)]
    >>> graph.vertices.add(v1)
    >>> graph.vertices.add(v2)
    >>> graph.edges.add(v1, v2, np.arange(10, dtype=np.byte))
    >>> graph.edges.get((v1, v2))
    array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], dtype=int8)
    >>> graph.edges.get((v1, (0, 3)))
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    KeyError:

)";

static const char* get_edges = R"(
    Get all vertices connected to one vertex

    Args:
        vertex(tuple): vertex unique identifier.

    Returns:
        vector of vertices (usable like a list)

    >>> graph.vertices.clear()
    >>> v1, v2, v3 = [(0, 1), (0, 2), (1, 3)]
    >>> _ = [graph.vertices.add(v) for v in [v1, v2, v3]]
    >>> graph.edges.add(v1, v2)
    >>> graph.edges.add(v1, v3)
    >>> for v in graph.edges.get(v1):
    ...   print(v)
    (0, 2)
    (1, 3)

)";

static const char* get_edges_filter = R"(
    Get vertices of a certain type connected to one vertex

    Args:
        vertex(tuple): vertex unique identifier.
        filter(int): vertex type.

    Returns:
        vector of vertices (usable like a list).

    >>> graph.vertices.clear()
    >>> v1, v2, v3 = [(0, 1), (0, 2), (1, 3)]
    >>> _ = [graph.vertices.add(v) for v in [v1, v2, v3]]
    >>> graph.edges.add(v1, v2)
    >>> graph.edges.add(v1, v3)
    >>> for v in graph.edges.get(v1, 0):
    ...   print(v)
    (0, 2)

)";

static const char* add_edge = R"(
    Add or overwrite an edge

    Args:
        vertex1(tuple): first vertex identifier.
        vertex2(tuple): second vertex identifier.
        commit(bool): whether uncommitted operations should be flushed or not.

    >>> graph.vertices.clear()
    >>> v1, v2 = [(0, 1), (0, 2)]
    >>> graph.vertices.add(v1)
    >>> graph.vertices.add(v2)
    >>> graph.edges.add(v1, v2)

)";

static const char* add_edge_payload = R"(
    Add or overwrite an edge with a payload attached

    Args:
        vertex1(tuple): first vertex identifier.
        vertex2(tuple): second vertex identifier.
        payload(np.array(dtype=np.byte)): array of bytes.
        commit(bool): whether uncommitted operations should be flushed or not.

    >>> graph.vertices.clear()
    >>> v1, v2 = [(0, 1), (0, 2)]
    >>> graph.vertices.add(v1)
    >>> graph.vertices.add(v2)
    >>> graph.edges.add(v1, v2, np.arange(10, dtype=np.byte))

)";

static const char* add_bulk = R"(
    Create an edge between a vertex and a list of other vertices

    Args:
        vertex(tuple): vertex from which all edges to create start.
        type(int): type of target vertices to connect to (the same of all target vertices).
        vertices(np.array(dtype=np.int64)): array of target vertices identifiers.
        create_vertices(bool): wether the target vertices are also created or not
            Any prior vertices with the same identifiers will be overwritten.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

static const char* add_bulk_payload = R"(
    Create an edge between a vertex and a list of other vertices that are also created

    Args:
        vertex(tuple): vertex from which all edges to create start.
        type(int): type of target vertices to connect to (the same of all target vertices).
        vertices(np.array(dtype=np.int64)): array of target vertices identifiers.
        vertex_payloads(list of np.array(dtype=np.int64)): payload of target vertices.
        create_vertices(bool): wether the target vertices are also created or not.
            Any prior vertices with the same identifiers will be overwritten.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

}  // namespace docstring


template <EdgeOrientation Orientation>
py::class_<basalt::Edges<Orientation>> register_graph_edges_class(
    py::module& m,
    const std::string& class_prefix = "") {
    return py::class_<basalt::Edges<Orientation>>(m,
                                                  (class_prefix + "Edges").c_str(),
                                                  docstring::edges_class)
        .def("__iter__",
             [](const basalt::Edges<Orientation>& edges) {
                 return py::make_iterator(edges.begin(), edges.end());
             },
             py::keep_alive<0, 1>())

        .def("__len__",
             [](const basalt::Edges<Orientation>& edges) {
                 std::size_t count{};
                 edges.count(count).raise_on_error();
                 return count;
             })

        .def("add",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2,
                bool commit) {
                 const auto status = edges.insert(vertex1, vertex2, commit);
                 status.raise_on_error();
             },
             "vertex1"_a,
             "vertex2"_a,
             "commit"_a = false,
             docstring::add_edge)

        .def("add",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2,
                py::array_t<char> data,
                bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'data' must be one");
                 }
                 const auto status = edges.insert(
                     vertex1, vertex2, data.data(), static_cast<std::size_t>(data.size()), commit);
                 status.raise_on_error();
             },
             "vertex1"_a,
             "vertex2"_a,
             "data"_a,
             "commit"_a = false,
             docstring::add_edge_payload)

        .def("add",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex,
                const basalt::vertex_t type,
                py::array_t<basalt::vertex_id_t> vertices,
                bool commit,
                bool create_vertices) {
                 if (vertices.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions of array 'vertices' must be one");
                 }
                 edges
                     .insert(vertex,
                             type,
                             vertices.data(),
                             static_cast<std::size_t>(vertices.size()),
                             create_vertices,
                             commit)
                     .raise_on_error();
             },
             "vertex"_a,
             "type"_a,
             "vertices"_a,
             "commit"_a = false,
             "create_vertices"_a = false,
             docstring::add_bulk)

        .def("add",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex,
                const basalt::vertex_t type,
                py::array_t<basalt::vertex_id_t> vertices,
                py::list vertex_payloads,
                bool commit,
                bool create_vertices) {
                 if (vertices.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions of array 'vertices' must be one");
                 }
                 if (static_cast<std::size_t>(vertices.size()) != vertex_payloads.size()) {
                     throw std::runtime_error("Number of vertices and vertex_payloads differ");
                 }
                 std::vector<const char*> vertex_payloads_data;
                 vertex_payloads_data.reserve(vertex_payloads.size());
                 std::vector<std::size_t> vertex_payloads_sizes;
                 vertex_payloads_sizes.reserve(vertex_payloads.size());
                 for (py::handle handle: vertex_payloads) {
                     py::array_t<char> vertex_payload = py::cast<py::array_t<char>>(handle);
                     vertex_payloads_data.push_back(vertex_payload.data());
                     vertex_payloads_sizes.push_back(
                         static_cast<std::size_t>(vertex_payload.size()));
                 }
                 edges
                     .insert(vertex,
                             type,
                             vertices.data(),
                             vertex_payloads_data.data(),
                             vertex_payloads_sizes.data(),
                             static_cast<std::size_t>(vertices.size()),
                             create_vertices,
                             commit)
                     .raise_on_error();
             },
             "vertex"_a,
             "type"_a,
             "vertices"_a,
             "vertex_payloads"_a,
             "commit"_a = false,
             "create_vertices"_a = false,
             docstring::add_bulk_payload)

        .def("__contains__",
             [](const basalt::Edges<Orientation>& edges, const basalt::edge_uid_t& edge) {
                 bool result = false;
                 edges.has(edge.first, edge.second, result).raise_on_error();
                 return result;
             },
             "edge"_a,
             "Check connectivity between 2 vertices")

        .def("get",
             [](const basalt::Edges<Orientation>& edges,
                const basalt::edge_uid_t& edge) -> py::object {
                 std::string data;
                 const auto& status = edges.get(edge, &data);
                 if (status.code == basalt::Status::missing_edge_code) {
                     throw py::key_error();
                 }
                 status.raise_on_error();
                 if (data.empty()) {
                     return py::none();
                 }
                 return std::move(basalt::to_py_array(data));
             },
             "edge"_a,
             docstring::get_data)

        .def("get",
             [](const basalt::Edges<Orientation>& edges, const basalt::vertex_uid_t& vertex) {
                 basalt::vertex_uids_t eax;
                 edges.get(vertex, eax).raise_on_error();
                 return eax;
             },
             "vertex"_a,
             docstring::get_edges)

        .def("get",
             [](const basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex,
                basalt::vertex_t filter) {
                 basalt::vertex_uids_t eax;
                 edges.get(vertex, filter, eax).raise_on_error();
                 return eax;
             },
             "vertex"_a,
             "filter"_a,
             docstring::get_edges_filter)

        .def("discard",
             [](basalt::Edges<Orientation>& edges,
                const basalt::edge_uid_t& edge,
                bool commit = false) {
                 edges.erase(edge.first, edge.second, commit).raise_on_error();
             },
             "edge"_a,
             "commit"_a = false,
             docstring::discard_edge)

        .def("discard",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex,
                bool commit = false) {
                 std::size_t removed;
                 edges.erase(vertex, removed, commit).raise_on_error();
                 return removed;
             },
             "vertex"_a,
             "commit"_a = false,
             docstring::discard_edges)

        .def("discard",
             [](basalt::Edges<Orientation>& edges,
                const basalt::vertex_uid_t& vertex,
                basalt::vertex_t filter,
                bool commit = false) {
                 std::size_t removed;
                 edges.erase(vertex, filter, removed, commit).raise_on_error();
                 return removed;
             },
             "vertex"_a,
             "filter"_a,
             "commit"_a = false,
             docstring::discard_edges_if);
}

void register_graph_edges(py::module& m) {
    register_graph_edges_class<EdgeOrientation::undirected>(m);
    register_graph_edges_class<EdgeOrientation::directed>(m, "Directed");
}

}  // namespace basalt
