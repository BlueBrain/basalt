/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
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
#include "config.hpp"
#include "graph_impl.hpp"
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

/** \brief docstring of Python symbols */
namespace docstring {

static const char* status = R"(
    Provide result of one or several graph operations

    Attributes:
        code(int): operation status code, 0 means success.
        message(str): optional message

    To use it:

    >>> status = basalt.Status(code=42)
    >>> status.code
    42
    >>> if status:
    ...   print('ok')
    ... else:
    ...   print('oops')
    ...
    oops

    A message for humans can be also specified

    >>> status = basalt.Status(code=42, message="the answer to every question")
    >>> status.message
    'the answer to every question'
)";

static const char* make_id = R"(
    Helper function to create vertex identifier

    Args:
        type(int): vertex type
        id(int): vertex identifier

    Returns:
        A tuple of 2 elements providing the type and id

    >>> basalt.make_id(42, 43)
    (42, 43)
)";

static const char* default_json_config = R"(
    Helper function to write a JSON file containing
    the default basalt database configuration.

    Args:
        type(str): path to JSON file to write
)";

static const char* status_raise_on_error = R"(
    Ensure status is ok

    Raises:
        RuntimeException: whenever status code is different than 0

    Returns:
        This instance

    >>> basalt.Status(code=0, message="all good").raise_on_error()
    Status(code=0, message="all good")
    >>> basalt.Status(code=42, message="actually...").raise_on_error()
    Traceback (most recent call last):
        ...
    RuntimeError: actually...
)";

static const char* graph = R"(
    Undirected Connectivity Graph

)";

static const char* directed_graph = R"(
    Directed Connectivity Graph

)";

static const char* graph_init = R"(
    Construct a graph object

    Args:
        path(str): path to rocksdb database on filesystem.
        Database is will be created if path does not exist.
)";

static const char* graph_init_with_config = R"(
    Construct a graph object

    Args:
        path(str): unexisting path to rocksdb database on filesystem to create
        config(str): path to a JSON file
)";

static const char* graph_commit = R"(
    Flush pending changes on disk

    Raises:
        RuntimeException: uppon error

    >>> graph.vertices.add((1, 42))
    >>> graph.commit()
)";

static const char* graph_edges = R"(
    Get wrapper around the edges of the graph

    Returns:
        instance of :py:class:`Edges`
)";

static const char* graph_vertices = R"(
    Get wrapper around the vertices of the graph

    Returns:
        instance of :py:class:`Vertices`
)";

static const char* graph_statistics = R"(
    Get RocksDB usage statistics as a string
)";

}  // namespace docstring


#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif                         // defined(__clang__)
PYBIND11_MODULE(_basalt, m) {  // NOLINT
    m.doc() = "Basic graph database backed by RocksDB key-value storage";
    m.attr("__rocksdb_version__") = rocksdb_version();
    m.attr("__version__") = basalt_version();

    m.def("make_id", &basalt::make_id, "type"_a, "id"_a, docstring::make_id);

    m.def("default_config_file",
          [](const std::string& path) {
              std::ofstream ostr(path);
              ostr << basalt::Config();
          },
          "path"_a,
          docstring::default_json_config);

    py::class_<basalt::Status>(m, "Status", docstring::status)
        .def(py::init([](int code, const std::string& message) {
                 return basalt::Status(static_cast<basalt::Status::Code>(code), message);
             }),
             "code"_a,
             "message"_a = std::string())
        .def_property_readonly("code",
                               [](const basalt::Status& status) {
                                   return static_cast<int>(status.code);
                               })
        .def_readonly("message", &basalt::Status::message)
        .def("raise_on_error", &basalt::Status::raise_on_error, docstring::status_raise_on_error)
        .def("__bool__",
             [](const basalt::Status& status) -> bool { return static_cast<bool>(status); })
        .def("__repr__", [](const basalt::Status& status) {
            std::ostringstream oss;
            oss << "Status(code=" << status.code << ", message=\"" << status.message << "\")";
            return oss.str();
        });

    py::class_<basalt::UndirectedGraph>(m, "UndirectedGraph", docstring::graph)
        .def(py::init<const std::string&>(), "path"_a, docstring::graph_init)
        .def(py::init<const std::string&, const std::string&>(),
             "path"_a,
             "config"_a,
             docstring::graph_init_with_config)
        .def_property_readonly("vertices",
                               &basalt::UndirectedGraph::vertices,
                               docstring::graph_vertices)
        .def_property_readonly("edges", &basalt::UndirectedGraph::edges, docstring::graph_edges)
        .def("commit",
             [](basalt::UndirectedGraph& graph) { graph.commit().raise_on_error(); },
             docstring::graph_commit)
        .def("statistics", &basalt::UndirectedGraph::statistics, docstring::graph_statistics);

    py::class_<basalt::DirectedGraph>(m, "DirectedGraph", docstring::directed_graph)
        .def(py::init<const std::string&>(), "path"_a, docstring::graph_init)
        .def(py::init<const std::string&, const std::string&>(),
             "path"_a,
             "config"_a,
             docstring::graph_init_with_config)
        .def_property_readonly("vertices",
                               &basalt::DirectedGraph::vertices,
                               docstring::graph_vertices)
        .def_property_readonly("edges", &basalt::DirectedGraph::edges, docstring::graph_edges)
        .def("commit",
             [](basalt::DirectedGraph& graph) { graph.commit().raise_on_error(); },
             docstring::graph_commit)
        .def("statistics", &basalt::DirectedGraph::statistics, docstring::graph_vertices);

    basalt::register_graph_edges(m);
    basalt::register_graph_vertices(m);
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif  // defined(__clang__)
