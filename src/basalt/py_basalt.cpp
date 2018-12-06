/**
 * \file src/basalt/py_basalt.cpp
 * Provides Python bindings for basalt library
 */

#include <iostream>
#include <string>
#include <vector>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "circuit_payloads.hpp"
#include "network_impl.hpp"
#include <basalt/version.hpp>

namespace py = pybind11;
using namespace pybind11::literals;
namespace b = basalt;
namespace c = basalt::circuit;

static const auto basalt_version = []() -> std::string {
    std::ostringstream oss;
    oss << BASALT_MAJOR << '.' << BASALT_MINOR << '.' << BASALT_PATCH;
    return oss.str();
}();

static const auto rocksdb_version = []() -> std::string {
    std::ostringstream oss;
    const auto version = b::rocksdb_version();
    oss << version[0] << '.' << version[1] << '.' << version[2];
    return oss.str();
};

// See
// https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#making-opaque-types
PYBIND11_MAKE_OPAQUE(c::point_vector_t);
PYBIND11_MAKE_OPAQUE(c::int_vector_t);
PYBIND11_MAKE_OPAQUE(c::float_point_t);

PYBIND11_MODULE(_basalt, m) {
    m.doc() = "Basic graph database backed by RocksDB key-value storage";
    m.attr("__rocksdb_version__") = rocksdb_version();
    m.attr("__version__") = basalt_version;

    // see
    // https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#binding-stl-containers
    py::bind_vector<c::point_vector_t>(m, "FloatPointVector",
                                       py::buffer_protocol());

    py::bind_vector<c::int_vector_t>(m, "IntegerPointVector",
                                     py::buffer_protocol());

    py::class_<c::float_point_t>(m, "Point")
        .def(py::init<>())
        .def(py::init([](c::float_point_t::value_type x,
                         c::float_point_t::value_type y,
                         c::float_point_t::value_type z) {
            return c::float_point_t{x, y, z};
        }))
        .def(py::init([](const std::list<c::float_point_t::value_type>& list) {
            c::float_point_t eax;
            if (list.size() != eax.size()) {
                throw std::runtime_error("Invalid list size");
            }
            std::copy(list.begin(), list.end(), eax.begin());
            return eax;
        }))

        .def("__len__", [](const c::float_point_t& p) { return p.size(); })
        .def("__iter__",
             [](const c::float_point_t& p) {
                 return py::make_iterator(p.begin(), p.end());
             },
             py::keep_alive<0, 1>())
        .def("__getitem__",
             [](const c::float_point_t& p, int index) { return p[index]; })
        .def("__setitem__",
             [](c::float_point_t& p, int index,
                c::float_point_t::value_type value) { p[index] = value; })

        .def("__repr__",
             [](const c::float_point_t& p) {
                 std::ostringstream oss;
                 oss << "basalt.Point(" << p[0] << ", " << p[1] << ", " << p[2]
                     << ")";
                 return oss.str();
             })
        .def_property("x", [](const c::float_point_t& p) { return p[0]; },
                      [](c::float_point_t& p,
                         c::float_point_t::value_type value) { p[0] = value; })

        .def_property("y", [](const c::float_point_t& p) { return p[1]; },
                      [](c::float_point_t& p,
                         c::float_point_t::value_type value) { p[1] = value; })

        .def_property("z", [](const c::float_point_t& p) { return p[2]; },
                      [](c::float_point_t& p,
                         c::float_point_t::value_type value) { p[2] = value; });

    py::class_<c::neuron_t>(m, "Neuron")
        .def(py::init<>())
        .def(py::init(&c::neuron_t::create), "gid"_a,
             "astro_idx"_a = py::array_t<uint32_t>(0),
             "syn_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &c::neuron_t::serialize)
        .def("deserialize", &c::neuron_t::deserialize)
        .def_readwrite("gid", &c::neuron_t::gid)
        .def_readwrite("astro_idx", &c::neuron_t::astro_idx)
        .def_readwrite("syn_idx", &c::neuron_t::syn_idx);

    py::class_<c::synapse_t>(m, "Synapse")
        .def(py::init<>())
        .def(py::init(&c::synapse_t::create), "pre_gid"_a = 0, "post_gid"_a = 0,
             "nrn_idx"_a = 0, "astro_idx"_a = py::array_t<uint32_t>(0),
             "is_excitatory"_a = false,
             "pre"_a = c::float_point_t({0., 0., 0.}),
             "post"_a = c::float_point_t({0., 0., 0.}), "mesh_filename"_a = "",
             "skeleton_filename"_a = "", "psd_area"_a = 0.)
        .def("serialize", &c::synapse_t::serialize)
        .def("deserialize", &c::synapse_t::deserialize)
        .def_readwrite("pre", &c::synapse_t::pre)
        .def_readwrite("pre_gid", &c::synapse_t::pre_gid)
        .def_readwrite("post_gid", &c::synapse_t::post_gid)
        .def_readwrite("nrn_idx", &c::synapse_t::nrn_idx)
        .def_readwrite("astro_idx", &c::synapse_t::astro_idx)
        .def_readwrite("is_excitatory", &c::synapse_t::is_excitatory)
        .def_readwrite("pre", &c::synapse_t::pre)
        .def_readwrite("post", &c::synapse_t::post)
        .def_readwrite("mesh_filename", &c::synapse_t::mesh_filename)
        .def_readwrite("skeleton_filename", &c::synapse_t::skeleton_filename)
        .def_readwrite("psd_area", &c::synapse_t::psd_area);

    py::class_<c::astrocyte_t>(m, "Astrocyte")
        .def(py::init<>())
        .def(py::init(&c::astrocyte_t::create), "astrocyte_id"_a = 0,
             "microdomain_id"_a = 0,
             "soma_center"_a = c::float_point_t({0., 0., 0.}),
             "soma_radius"_a = 0., "name"_a = "", "mtype"_a = "",
             "morphology_filename"_a = "",
             "synapses_idx"_a = py::array_t<uint32_t>(0),
             "neurons_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &c::astrocyte_t::serialize)
        .def("deserialize", &c::astrocyte_t::deserialize)
        .def_readwrite("astrocyte_id", &c::astrocyte_t::astrocyte_id)
        .def_readwrite("microdomain_id", &c::astrocyte_t::microdomain_id)
        .def_readwrite("soma_center", &c::astrocyte_t::soma_center)
        .def_readwrite("soma_radius", &c::astrocyte_t::soma_radius)
        .def_readwrite("name", &c::astrocyte_t::name)
        .def_readwrite("mtype", &c::astrocyte_t::mtype)
        .def_readwrite("morphology_filename",
                       &c::astrocyte_t::morphology_filename)
        .def_readwrite("synapses_idx", &c::astrocyte_t::synapses_idx)
        .def_readwrite("neurons_idx", &c::astrocyte_t::neurons_idx);

    py::class_<c::microdomain_t>(m, "MicroDomain")
        .def(py::init<>())
        .def(py::init(&c::microdomain_t::create), "microdomain_id"_a = 0,
             "astrocyte_id"_a = 0, "neighbors"_a = py::array_t<uint32_t>(0),
             "vertex_coordinates"_a = py::array_t<float>(0),
             "triangles"_a = py::array_t<uint32_t>(0),
             "centroid"_a = c::float_point_t({0., 0., 0.}), "area"_a = 0.,
             "volume"_a = 0., "mesh_filename"_a = "",
             "neurons_idx"_a = py::array_t<uint32_t>(0),
             "synapses_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &c::microdomain_t::serialize)
        .def("deserialize", &c::microdomain_t::deserialize)

        .def_readwrite("microdomain_id", &c::microdomain_t::microdomain_id)
        .def_readwrite("astrocyte_id", &c::microdomain_t::astrocyte_id)
        .def_readwrite("neighbors", &c::microdomain_t::neighbors)
        .def_readwrite("vertex_coordinates",
                       &c::microdomain_t::vertex_coordinates)
        .def_readwrite("triangles", &c::microdomain_t::triangles)
        .def_readwrite("centroid", &c::microdomain_t::centroid)
        .def_readwrite("area", &c::microdomain_t::area)
        .def_readwrite("volume", &c::microdomain_t::volume)
        .def_readwrite("mesh_filename", &c::microdomain_t::mesh_filename)
        .def_readwrite("neurons_idx", &c::microdomain_t::neurons_idx)
        .def_readwrite("synapses_idx", &c::microdomain_t::synapses_idx);

    py::class_<c::segment_t>(m, "Segment")
        .def(py::init<>())
        .def(py::init<>(&c::segment_t::create), "section_id"_a = 0,
             "segment_id"_a = 0, "type"_a = 0, "x1"_a = 0., "y1"_a = 0.,
             "z1"_a = 0., "r1"_a = 0., "x2"_a = 0., "y2"_a = 0., "z2"_a = 0.,
             "r2"_a = 0.)
        .def("serialize", &c::segment_t::serialize)
        .def("deserialize", &c::segment_t::deserialize)
        .def_readwrite("section_id", &c::segment_t::section_id)
        .def_readwrite("segment_id", &c::segment_t::segment_id)
        .def_readwrite("type", &c::segment_t::type)
        .def_readwrite("x1", &c::segment_t::x1)
        .def_readwrite("y1", &c::segment_t::y1)
        .def_readwrite("z1", &c::segment_t::z1)
        .def_readwrite("r1", &c::segment_t::r1)
        .def_readwrite("x2", &c::segment_t::x2)
        .def_readwrite("y2", &c::segment_t::y2)
        .def_readwrite("z2", &c::segment_t::z2)
        .def_readwrite("r2", &c::segment_t::r2);

    py::class_<c::edge_astrocyte_segment_t>(m, "EdgeAstrocyteSegment")
        .def(py::init<>())
        .def(py::init<>(&c::edge_astrocyte_segment_t::create),
             "astrocyte"_a = c::float_point_t({0., 0., 0.}),
             "vasculature"_a = c::float_point_t({0., 0., 0.}))
        .def("serialize", &c::edge_astrocyte_segment_t::serialize)
        .def("deserialize", &c::edge_astrocyte_segment_t::deserialize)
        .def_readwrite("astrocyte", &c::edge_astrocyte_segment_t::astrocyte)
        .def_readwrite("vasculature",
                       &c::edge_astrocyte_segment_t::vasculature);

    m.def("make_id", &b::make_id, "node_id_t constructor helper function");

    py::class_<b::status_t>(m, "Status")
        .def_readonly("code", &b::status_t::code)
        .def_readonly("message", &b::status_t::message)
        .def("raise_on_error", &b::status_t::raise_on_error);

    py::class_<b::network_t>(m, "Network")
        .def(py::init<const std::string&>())
        .def_property_readonly("nodes", &b::network_t::nodes)
        .def_property_readonly("connections", &b::network_t::connections)
        .def("commit", &b::network_t::commit);

    py::class_<b::connections_t>(m, "Connections")
        .def("insert",
             [](b::connections_t& connections, const b::node_uid_t& node1,
                const b::node_uid_t& node2, bool commit) {
                 const auto status = connections.insert(node1, node2, commit);
                 status.raise_on_error();
             },
             "Create an edge between 2 nodes", "node1"_a, "node2"_a,
             "commit"_a = false)

        .def("insert",
             [](b::connections_t& connections, const b::node_uid_t& node1,
                const b::node_uid_t& node2, py::array_t<char> data,
                bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions must be one");
                 }
                 const auto status = connections.insert(
                     node1, node2, data.data(), data.size(), commit);
                 status.raise_on_error();
             },
             "insert a node in the graph", "node1"_a, "node2"_a, "data"_a,
             "commit"_a = false)

        .def("has",
             [](const b::connections_t& connections, const b::node_uid_t& node1,
                const b::node_uid_t& node2) {
                 bool result;
                 connections.has(node1, node2, result).raise_on_error();
                 return result;
             },
             "Check connectivity between 2 nodes", "node1"_a, "node2"_a)

        .def(
            "get",
            [](const b::connections_t& connections, const b::node_uid_t& node) {
                b::node_uids_t eax;
                connections.get(node, eax).raise_on_error();
                return eax;
            },
            "get nodes connected to one node", "node"_a)

        .def("get",
             [](const b::connections_t& connections, const b::node_uid_t& node,
                b::node_t filter) {
                 b::node_uids_t eax;
                 connections.get(node, filter, eax).raise_on_error();
                 return eax;
             },
             "get nodes of a specific type connected to one node", "node"_a,
             "filter"_a)

        .def("erase",
             [](b::connections_t& connections, const b::node_uid_t& node1,
                const b::node_uid_t& node2, bool commit = false) {
                 connections.erase(node1, node2, commit).raise_on_error();
             },
             "remove connection between 2 nodes", "node1"_a, "node2"_a,
             "commit"_a = false)

        .def("erase",
             [](b::connections_t& connections, const b::node_uid_t& node,
                bool commit = false) {
                 std::size_t removed;
                 connections.erase(node, removed, commit).raise_on_error();
                 return removed;
             },
             "remove all connections of a node", "node"_a, "commit"_a = false)

        .def("erase",
             [](b::connections_t& connections, const b::node_uid_t& node,
                b::node_t filter, bool commit = false) {
                 std::size_t removed;
                 connections.erase(node, filter, removed, commit)
                     .raise_on_error();
                 return removed;
             },
             "remove all connections of a node", "node"_a, "filter"_a,
             "commit"_a = false);

    py::class_<b::nodes_t>(m, "Nodes")
        .def("__iter__",
             [](const b::nodes_t& nodes) {
                 return py::make_iterator(nodes.begin(), nodes.end());
             },
             py::keep_alive<0, 1>())

        .def("insert",
             [](b::nodes_t& nodes, b::node_t type, b::node_id_t id,
                bool commit = false) {
                 b::node_uid_t uid;
                 const auto status = nodes.insert(type, id, uid, commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a node in the graph", "type"_a, "id"_a, "commit"_a = false)

        .def("insert",
             [](b::nodes_t& nodes, b::node_t type, b::node_id_t id,
                py::array_t<char> data, bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error(
                         "Number of dimensions must be one");
                 }
                 b::node_uid_t uid;
                 const auto status = nodes.insert(type, id, data.data(),
                                                  data.size(), uid, commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a node in the graph", "node"_a, "id"_a, "data"_a,
             "commit"_a = false)

        .def("get",
             [](b::nodes_t& nodes, b::node_uid_t node) -> py::object {
                 std::string data;
                 auto const& status = nodes.get(node, &data);
                 if (status.code == b::status_t::missing_node_code) {
                     return py::none();
                 }
                 status.raise_on_error();
                 auto array = py::array_t<char>(data.size());
                 auto buffer = array.request(true);
                 strncpy(reinterpret_cast<char*>(buffer.ptr), data.data(),
                         data.size());
                 return std::move(array);
             },
             "Retrieve a node from the graph", "node"_a)

        .def("has",
             [](b::nodes_t& nodes, b::node_t type, b::node_id_t id) {
                 bool result = false;
                 const auto uid = b::make_id(type, id);
                 auto const status = nodes.has(uid, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a node in the graph", "type"_a, "id"_a)

        .def("has",
             [](b::nodes_t& nodes, b::node_uid_t node) {
                 bool result = false;
                 auto const status = nodes.has(node, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a node in the graph", "node"_a)

        .def("erase",
             [](b::nodes_t& nodes, b::node_uid_t node, bool commit) {
                 auto const status = nodes.erase(node, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a node from the graph", "node"_a, "commit"_a = false)

        .def("erase",
             [](b::nodes_t& nodes, b::node_t type, b::node_id_t id,
                bool commit) {
                 auto const node = b::make_id(type, id);
                 auto const status = nodes.erase(node, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a node from the graph", "type"_a, "id"_a,
             "commit"_a = false);
}
