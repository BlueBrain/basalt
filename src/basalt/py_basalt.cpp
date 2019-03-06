/**
 * \file src/basalt/py_basalt.cpp
 * Provides Python bindings for basalt library
 */

#include <iostream>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "basalt/version.hpp"
#include "graph_impl.hpp"
#include "py_circuit_payloads.hpp"
#include "py_graph_edges.hpp"
#include "py_graph_vertices.hpp"
#include "py_helpers.hpp"

namespace py = pybind11;
using pybind11::literals::operator""_a;

static const std::string& basalt_version() {
    static const auto basalt_version = []() -> std::string {
        std::ostringstream oss;
        oss << BASALT_MAJOR << '.' << BASALT_MINOR << '.' << BASALT_PATCH;
        return oss.str();
    }();
    return basalt_version;
}

static const std::string& rocksdb_version() {
    static const auto rocksdb_version = []() -> std::string {
        std::ostringstream oss;
        const auto version = basalt::rocksdb_version();
        oss << version[0] << '.' << version[1] << '.' << version[2];
        return oss.str();
    }();
    return rocksdb_version;
}

static const char* status_raise_on_error = R"(Ensure status is ok

Raises:
    RuntimeException: if status is not ok.

Returns:
    This instance

)";


#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif                         // defined(__clang__)
PYBIND11_MODULE(_basalt, m) {  // NOLINT
    m.doc() = "Basic graph database backed by RocksDB key-value storage";
    m.attr("__rocksdb_version__") = rocksdb_version();
    m.attr("__version__") = basalt_version();

    m.def("make_id", &basalt::make_id, "vertex_id_t constructor helper function");

    py::class_<basalt::Status>(m, "Status")
        .def_readonly("code", &basalt::Status::code, "status code")
        .def_readonly("message", &basalt::Status::message, "status description for humans")
        .def("raise_on_error", &basalt::Status::raise_on_error, status_raise_on_error);

    py::class_<basalt::Graph>(m, "Graph")
        .def(py::init<const std::string&>())
        .def_property_readonly("vertices", &basalt::Graph::vertices,
                               "get wrapper object around vertices")
        .def_property_readonly("edges", &basalt::Graph::edges, "get wrapper object around edges")
        .def("commit", &basalt::Graph::commit, "apply pending changes")
        .def("statistics", &basalt::Graph::statistics, "Returns database statistics usage");

    basalt::register_circuit_payloads_bindings(m);
    basalt::register_graph_edges(m);
    basalt::register_graph_vertices(m);
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif  // defined(__clang__)
