#include <vector>

#include <pybind11/stl.h>

#include "basalt/edges.hpp"
#include "py_helpers.hpp"

namespace py = pybind11;
using pybind11::literals::operator""_a;

namespace basalt {

void register_graph_edges(py::module& m) {
    py::class_<basalt::Edges>(m, "Edges")
        .def("insert",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2, bool commit) {
                 const auto status = edges.insert(vertex1, vertex2, commit);
                 status.raise_on_error();
             },
             "Create an edge between 2 existing vertices", "v1"_a, "v2"_a, "commit"_a = false)

        .def("insert",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2, py::array_t<char> data, bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'data' must be one");
                 }
                 const auto status = edges.insert(vertex1, vertex2, data.data(),
                                                  static_cast<std::size_t>(data.size()), commit);
                 status.raise_on_error();
             },
             "Create an edge with a payload between 2 existing vertices", "v1"_a, "v2"_a, "data"_a,
             "commit"_a = false)

        .def("insert",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex,
                const basalt::vertex_t type, py::array_t<uint64_t> vertices, bool commit,
                bool create_vertices) {
                 if (vertices.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions of array 'vertices' must be one");
                 }
                 edges
                     .insert(vertex, type, vertices.data(),
                             static_cast<std::size_t>(vertices.size()), create_vertices, commit)
                     .raise_on_error();
             },
             "Create an edge between a vertex and a list of other vertices", "vertex"_a, "type"_a,
             "vertices"_a, "commit"_a = false, "create_vertices"_a = false)

        .def("insert",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex,
                const basalt::vertex_t type, py::array_t<uint64_t> vertices,
                py::list vertex_payloads, bool commit, bool create_vertices) {
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
                     .insert(vertex, type, vertices.data(), vertex_payloads_data.data(),
                             vertex_payloads_sizes.data(),
                             static_cast<std::size_t>(vertices.size()), create_vertices, commit)
                     .raise_on_error();
             },
             "Create an edge between a vertex and a list of other vertices", "vertex"_a, "type"_a,
             "vertices"_a, "vertex_payloads"_a, "commit"_a = false, "create_vertices"_a = false)

        .def("has",
             [](const basalt::Edges& edges, const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2) {
                 bool result;
                 edges.has(vertex1, vertex2, result).raise_on_error();
                 return result;
             },
             "Check connectivity between 2 vertices", "v1"_a, "v2"_a)

        .def("get",
             [](const basalt::Edges& edges, const basalt::vertex_uid_t& vertex) {
                 basalt::vertex_uids_t eax;
                 edges.get(vertex, eax).raise_on_error();
                 return eax;
             },
             "get vertices connected to one vertex", "vertex"_a)

        .def("get",
             [](const basalt::Edges& edges, const basalt::vertex_uid_t& vertex,
                basalt::vertex_t filter) {
                 basalt::vertex_uids_t eax;
                 edges.get(vertex, filter, eax).raise_on_error();
                 return eax;
             },
             "get vertices of a specific type connected to one vertex", "vertex"_a, "filter"_a)

        .def("erase",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex1,
                const basalt::vertex_uid_t& vertex2,
                bool commit = false) { edges.erase(vertex1, vertex2, commit).raise_on_error(); },
             "remove edge between 2 vertices", "vertex1"_a, "vertex2"_a, "commit"_a = false)

        .def("erase",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex, bool commit = false) {
                 std::size_t removed;
                 edges.erase(vertex, removed, commit).raise_on_error();
                 return removed;
             },
             "remove all edges of a vertex", "vertex"_a, "commit"_a = false)

        .def("erase",
             [](basalt::Edges& edges, const basalt::vertex_uid_t& vertex, basalt::vertex_t filter,
                bool commit = false) {
                 std::size_t removed;
                 edges.erase(vertex, filter, removed, commit).raise_on_error();
                 return removed;
             },
             "remove all edges of a vertex", "vertex"_a, "filter"_a, "commit"_a = false);
}

}  // namespace basalt
