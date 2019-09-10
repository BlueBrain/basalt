/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <vector>

#include "basalt/vertex_iterator.hpp"
#include "basalt/vertices.hpp"
#include "py_graph_vertices.hpp"
#include "py_helpers.hpp"

namespace py = pybind11;
using pybind11::literals::operator""_a;

namespace basalt {

namespace docstring {

static const char* vertices_class = R"(
    Manage vertices of the graph

    A vertex is a tuple made of 2 integers
    representing the vertex type and identifier.

    Optionally, a byte-array can be attached to a vertex.

    >>> graph.vertices.clear()
    >>> graph.vertices.add((0, 1))
    >>> graph.vertices.add((0, 2))
    >>> len(graph.vertices)
    2

    >>> (0, 1) in graph.vertices
    True
    >>> (0, 3) not in graph.vertices
    True
    >>> for vertex in graph.vertices:
    ...   print(vertex)
    (0, 1)
    (0, 2)

    >>> graph.vertices.discard((0, 3))
    >>> graph.vertices.discard((0, 1))
    >>> (0, 1) in graph.vertices
    False

)";

static const char* discard = R"(
    Remove the given vertex if present

    Args:
        type(int): vertex type
        id(int): vertex identifier

)";

static const char* len = R"(
    Returns:
        number of vertices

)";

static const char* count_type = R"(
    Get number of vertices of a certain type

    Args:
        type(int): vertex type.

    Returns:
        Number of vertices of the given type

)";

static const char* add = R"(
    Insert a vertex in the graph

    Args:
        vertex(tuple): vertex unique identifier.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

static const char* add_payload = R"(
    Insert a vertex in the graph

    Args:
        vertex(tuple): vertex unique identifier.
        data(numpy.array(dtype=numpy.byte)): payload attached to the vertex.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

static const char* add_bulk = R"(
    Insert several vertices in the graph all at once",

    Args:
        types(np.array(dtype=np.int32): 1-dimensional array of vertex types.
        ids(np.array(dtype=np.int64)): 1-dimensional array of vertex identifiers.
        payloads: optional list of vertex payload.
        commit(bool): whether uncommitted operations should be flushed or not.

)";

static const char* clear = R"(
    Remove all vertices of the graph along with their edges

    Args:
        commit(bool): whether uncommitted operations should be flushed or not.

    >>> graph.vertices.add((0, 1))
    >>> graph.vertices.clear()
    >>> len(graph.vertices)
    0

)";

static const char* get = R"(
    Retrieve a vertex payload from the graph

    Args:
        vertex(tuple): vertex unique identifier.

    Returns:
        vertex payload if vertex exists and has a payload, None
        otherwise.

    >>> graph.vertices.add((0, 1), np.arange(10, dtype=np.byte))
    >>> graph.vertices.add((0, 2))
    >>> graph.vertices.get((0, 1))
    array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], dtype=int8)
    >>> graph.vertices.get((0, 2)) is None
    True

)";

static const char* getitem = R"(
    Retrieve a vertex payload from the graph

    Args:
        vertex(tuple): vertex unique identifier.

    Raises:
        KeyError: if vertex does not exist

    Returns:
        vertex payload if vertex has a payload, None otherwise.

    >>> graph.vertices.add((0, 1), np.arange(10, dtype=np.byte))
    >>> graph.vertices.add((0, 2))
    >>> graph.vertices.get((0, 1))
    array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], dtype=int8)
    >>> graph.vertices.get((0, 2))
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    KeyError:

)";

}  // namespace docstring

template <EdgeOrientation Orientation>
py::class_<basalt::Vertices<Orientation>> register_graph_vertices_class(
    py::module& m,
    const std::string& class_prefix = "") {
    return py::class_<basalt::Vertices<Orientation>>(m,
                                                     (class_prefix + "Vertices").c_str(),
                                                     docstring::vertices_class)
        .def("__iter__",
             [](const basalt::Vertices<Orientation>& vertices) {
                 return py::make_iterator(vertices.begin(), vertices.end());
             },
             py::keep_alive<0, 1>())

        .def("__len__",
             [](const basalt::Vertices<Orientation>& vertices) {
                 std::size_t count;
                 vertices.count(count).raise_on_error();
                 return count;
             },
             docstring::len)

        .def("count",
             [](const basalt::Vertices<Orientation>& vertices,
                basalt::vertex_t type) -> std::size_t {
                 std::size_t count;
                 vertices.count(type, count).raise_on_error();
                 return count;
             },
             "type"_a,
             docstring::count_type)

        .def("add",
             [](basalt::Vertices<Orientation>& vertices,
                basalt::vertex_uid_t vertex,
                bool commit = false) {
                 const auto status = vertices.insert(vertex, commit);
                 status.raise_on_error();
             },
             "vertex"_a,
             "commit"_a = false,
             docstring::add)

        .def("add",
             [](basalt::Vertices<Orientation>& vertices,
                const basalt::vertex_uid_t& vertex,
                py::array_t<char> data,
                bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions must be one");
                 }
                 const auto status = vertices.insert(vertex,
                                                     data.data(),
                                                     static_cast<std::size_t>(data.size()),
                                                     commit);
                 status.raise_on_error();
             },
             "vertex"_a,
             "data"_a,
             "commit"_a = false,
             docstring::add_payload)

        /// TODO: replace types and ids by numpy.array([(0, 1), (2,3)], dtype="int32,int64")
        .def("add",
             [](basalt::Vertices<Orientation>& instance,
                py::array_t<basalt::vertex_t> types,
                py::array_t<basalt::vertex_id_t> ids,
                py::list payloads,
                bool commit = false) {
                 if (ids.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'ids' must be one");
                 }
                 if (types.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'types' must be one");
                 }
                 if (ids.size() != types.size()) {
                     throw std::runtime_error("Number of types and ids differ");
                 }
                 if (payloads.size() != 0) {
                     if (static_cast<std::size_t>(ids.size()) != payloads.size()) {
                         throw std::runtime_error("Number of ids and payloads differ");
                     }
                 }
                 std::vector<const char*> payloads_data;
                 std::vector<std::size_t> payloads_sizes;
                 payloads_data.reserve(payloads.size());
                 payloads_sizes.reserve(payloads.size());
                 for (py::handle list_item: payloads) {
                     py::array_t<char> payload = py::cast<py::array_t<char>>(list_item);
                     payloads_data.push_back(payload.data());
                     payloads_sizes.push_back(static_cast<std::size_t>(payload.size()));
                 }
                 if (payloads_data.empty()) {
                     instance
                         .insert(types.data(),
                                 ids.data(),
                                 nullptr,
                                 nullptr,
                                 static_cast<std::size_t>(ids.size()),
                                 commit)
                         .raise_on_error();
                 } else {
                     instance
                         .insert(types.data(),
                                 ids.data(),
                                 payloads_data.data(),
                                 payloads_sizes.data(),
                                 static_cast<std::size_t>(ids.size()),
                                 commit)
                         .raise_on_error();
                 }
             },
             "types"_a,
             "ids"_a,
             "payloads"_a = py::list(),
             "commit"_a = false,
             docstring::add_bulk)

        .def("get",
             [](basalt::Vertices<Orientation>& vertices,
                const basalt::vertex_uid_t& vertex) -> py::object {
                 std::string data;
                 auto const& status = vertices.get(vertex, &data);
                 if (status.code == basalt::Status::missing_vertex_code) {
                     return py::none();
                 }
                 status.raise_on_error();
                 if (data.empty()) {
                     return py::none();
                 }
                 return std::move(basalt::to_py_array(data));
             },
             "vertex"_a,
             docstring::get)

        .def("__getitem__",
             [](basalt::Vertices<Orientation>& vertices,
                const basalt::vertex_uid_t& vertex) -> py::object {
                 std::string data;
                 auto const& status = vertices.get(vertex, &data);
                 if (status.code == basalt::Status::missing_vertex_code) {
                     throw py::key_error();
                 }
                 status.raise_on_error();
                 if (data.empty()) {
                     return py::none();
                 }
                 return std::move(basalt::to_py_array(data));
             },
             "vertex"_a,
             docstring::getitem)


        .def("clear",
             [](basalt::Vertices<Orientation>& vertices, bool commit = false) {
                 vertices.clear(commit).raise_on_error();
             },
             "commit"_a = false,
             docstring::clear)

        .def("__contains__",
             [](basalt::Vertices<Orientation>& vertices, const basalt::vertex_uid_t& vertex) {
                 bool result = false;
                 auto const status = vertices.has(vertex, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a vertex in the graph",
             "vertex"_a)

        .def("discard",
             [](basalt::Vertices<Orientation>& vertices,
                const basalt::vertex_uid_t& vertex,
                bool commit = false) { vertices.erase(vertex, commit).raise_on_error(); },
             "vertex"_a,
             "commit"_a = false,
             docstring::discard);
}

void register_graph_vertices(py::module& m) {
    register_graph_vertices_class<EdgeOrientation::directed>(m, "Directed");
    register_graph_vertices_class<EdgeOrientation::undirected>(m);
}

}  // namespace basalt
