#include <vector>

#include "basalt/vertex_iterator.hpp"
#include "basalt/vertices.hpp"
#include "py_helpers.hpp"

namespace py = pybind11;
using pybind11::literals::operator""_a;

namespace basalt {

void register_graph_vertices(py::module& m) {
    py::class_<basalt::Vertices>(m, "vertices")
        .def("__iter__",
             [](const basalt::Vertices& vertices) {
                 return py::make_iterator(vertices.begin(), vertices.end());
             },
             py::keep_alive<0, 1>())

        .def("insert",
             [](basalt::Vertices& vertices, basalt::vertex_t type, basalt::vertex_id_t id,
                bool commit = false) {
                 basalt::vertex_uid_t uid;
                 const auto status = vertices.insert(type, id, uid, commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a vertex in the graph", "type"_a, "id"_a, "commit"_a = false)

        .def("insert",
             [](basalt::Vertices& vertices, basalt::vertex_t type, basalt::vertex_id_t id,
                py::array_t<char> data, bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions must be one");
                 }
                 basalt::vertex_uid_t uid;
                 const auto status = vertices.insert(type, id, data.data(),
                                                     static_cast<std::size_t>(data.size()), uid,
                                                     commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a vertex in the graph", "vertex"_a, "id"_a, "data"_a, "commit"_a = false)

        .def("insert",
             [](basalt::Vertices& instance, py::array_t<basalt::vertex_t> types,
                py::array_t<basalt::vertex_id_t> vertices, py::list payloads, bool commit = false) {
                 if (vertices.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions of array 'vertices' must be one");
                 }
                 if (types.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'types' must be one");
                 }
                 if (vertices.size() != types.size()) {
                     throw std::runtime_error("Number of types and vertices differ");
                 }
                 if (payloads.size() != 0) {
                     if (static_cast<std::size_t>(vertices.size()) != payloads.size()) {
                         throw std::runtime_error("Number of vertices and payloads differ");
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
                         .insert(types.data(), vertices.data(), nullptr, nullptr,
                                 static_cast<std::size_t>(vertices.size()), commit)
                         .raise_on_error();
                 } else {
                     instance
                         .insert(types.data(), vertices.data(), payloads_data.data(),
                                 payloads_sizes.data(), static_cast<std::size_t>(vertices.size()),
                                 commit)
                         .raise_on_error();
                 }
             },
             "insert several vertices in the graph", "types"_a, "vertices"_a, "payloads"_a,
             "commit"_a = false)

        .def("get",
             [](basalt::Vertices& vertices, basalt::vertex_uid_t vertex) -> py::object {
                 std::string data;
                 auto const& status = vertices.get(vertex, &data);
                 if (status.code == basalt::Status::missing_vertex_code) {
                     return py::none();
                 }
                 status.raise_on_error();
                 return std::move(basalt::to_py_array(data));
             },
             "Retrieve a vertex from the graph", "vertex"_a)

        .def("has",
             [](basalt::Vertices& vertices, basalt::vertex_t type, basalt::vertex_id_t id) {
                 bool result = false;
                 const auto uid = basalt::make_id(type, id);
                 auto const status = vertices.has(uid, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a vertex in the graph", "type"_a, "id"_a)

        .def("has",
             [](basalt::Vertices& vertices, basalt::vertex_uid_t vertex) {
                 bool result = false;
                 auto const status = vertices.has(vertex, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a vertex in the graph", "vertex"_a)

        .def("erase",
             [](basalt::Vertices& vertices, basalt::vertex_uid_t vertex, bool commit) {
                 auto const status = vertices.erase(vertex, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a vertex from the graph", "vertex"_a, "commit"_a = false)

        .def("erase",
             [](basalt::Vertices& vertices, basalt::vertex_t type, basalt::vertex_id_t id,
                bool commit) {
                 auto const vertex = basalt::make_id(type, id);
                 auto const status = vertices.erase(vertex, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a vertex from the graph", "type"_a, "id"_a, "commit"_a = false);
}

}  // namespace basalt
