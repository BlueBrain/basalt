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
#include "circuit_payloads.hpp"
#include "network_impl.hpp"

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

// See
// https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#making-opaque-types
PYBIND11_MAKE_OPAQUE(circuit::point_vector_t);
PYBIND11_MAKE_OPAQUE(circuit::int_vector_t);
PYBIND11_MAKE_OPAQUE(circuit::float_point_t);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif                         // defined(__clang__)
PYBIND11_MODULE(_basalt, m) {  // NOLINT
    m.doc() = "Basic graph database backed by RocksDB key-value storage";
    m.attr("__rocksdb_version__") = rocksdb_version();
    m.attr("__version__") = basalt_version();

    // see
    // https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#binding-stl-containers
    py::bind_vector<circuit::point_vector_t>(m, "FloatPointVector", py::buffer_protocol());

    py::bind_vector<circuit::int_vector_t>(m, "IntegerPointVector", py::buffer_protocol());

    py::class_<circuit::float_point_t>(m, "Point")
        .def(py::init<>())
        .def(py::init([](circuit::float_point_t::value_type x, circuit::float_point_t::value_type y,
                         circuit::float_point_t::value_type z) {
            return circuit::float_point_t{x, y, z};
        }))
        .def(py::init([](const std::list<circuit::float_point_t::value_type>& list) {
            circuit::float_point_t eax;
            if (list.size() != eax.size()) {
                throw std::runtime_error("Invalid list size");
            }
            std::copy(list.begin(), list.end(), eax.begin());
            return eax;
        }))

        .def("__len__", [](const circuit::float_point_t& p) { return p.size(); })
        .def("__iter__",
             [](const circuit::float_point_t& p) { return py::make_iterator(p.begin(), p.end()); },
             py::keep_alive<0, 1>())
        .def("__getitem__", [](const circuit::float_point_t& p,
                               int index) { return p[static_cast<std::size_t>(index)]; })
        .def("__setitem__",
             [](circuit::float_point_t& p, int index, circuit::float_point_t::value_type value) {
                 p[static_cast<std::size_t>(index)] = value;
             })

        .def("__repr__",
             [](const circuit::float_point_t& p) {
                 std::ostringstream oss;
                 oss << "basalt.Point(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
                 return oss.str();
             })
        .def_property("x", [](const circuit::float_point_t& p) { return p[0]; },
                      [](circuit::float_point_t& p, circuit::float_point_t::value_type value) {
                          p[0] = value;
                      })

        .def_property("y", [](const circuit::float_point_t& p) { return p[1]; },
                      [](circuit::float_point_t& p, circuit::float_point_t::value_type value) {
                          p[1] = value;
                      })

        .def_property("z", [](const circuit::float_point_t& p) { return p[2]; },
                      [](circuit::float_point_t& p, circuit::float_point_t::value_type value) {
                          p[2] = value;
                      });

    py::class_<circuit::neuron_t>(m, "Neuron")
        .def(py::init<>())
        .def(py::init(&circuit::neuron_t::create), "gid"_a,
             "astro_idx"_a = py::array_t<uint32_t>(0), "syn_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &basalt::serialize<circuit::neuron_t>)
        .def("deserialize", &basalt::deserialize<circuit::neuron_t>)
        .def_readwrite("gid", &circuit::neuron_t::gid)
        .def_readwrite("astro_idx", &circuit::neuron_t::astro_idx)
        .def_readwrite("syn_idx", &circuit::neuron_t::syn_idx);

    py::class_<circuit::synapse_t>(m, "Synapse")
        .def(py::init<>())
        .def(py::init(&circuit::synapse_t::create), "pre_gid"_a = 0, "post_gid"_a = 0,
             "nrn_idx"_a = 0, "astro_idx"_a = py::array_t<uint32_t>(0), "is_excitatory"_a = false,
             "pre"_a = circuit::float_point_t({0., 0., 0.}),
             "post"_a = circuit::float_point_t({0., 0., 0.}), "mesh_filename"_a = "",
             "skeleton_filename"_a = "", "psd_area"_a = 0.)
        .def("serialize", &basalt::serialize<circuit::synapse_t>)
        .def("deserialize", &basalt::deserialize<circuit::synapse_t>)
        .def_readwrite("pre", &circuit::synapse_t::pre)
        .def_readwrite("pre_gid", &circuit::synapse_t::pre_gid)
        .def_readwrite("post_gid", &circuit::synapse_t::post_gid)
        .def_readwrite("nrn_idx", &circuit::synapse_t::nrn_idx)
        .def_readwrite("astro_idx", &circuit::synapse_t::astro_idx)
        .def_readwrite("is_excitatory", &circuit::synapse_t::is_excitatory)
        .def_readwrite("pre", &circuit::synapse_t::pre)
        .def_readwrite("post", &circuit::synapse_t::post)
        .def_readwrite("mesh_filename", &circuit::synapse_t::mesh_filename)
        .def_readwrite("skeleton_filename", &circuit::synapse_t::skeleton_filename)
        .def_readwrite("psd_area", &circuit::synapse_t::psd_area);

    py::class_<circuit::astrocyte_t>(m, "Astrocyte")
        .def(py::init<>())
        .def(py::init(&circuit::astrocyte_t::create), "astrocyte_id"_a = 0, "microdomain_id"_a = 0,
             "soma_center"_a = circuit::float_point_t({0., 0., 0.}), "soma_radius"_a = 0.,
             "name"_a = "", "mtype"_a = "", "morphology_filename"_a = "",
             "synapses_idx"_a = py::array_t<uint32_t>(0),
             "neurons_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &basalt::serialize<circuit::astrocyte_t>)
        .def("deserialize", &basalt::deserialize<circuit::astrocyte_t>)
        .def_readwrite("astrocyte_id", &circuit::astrocyte_t::astrocyte_id)
        .def_readwrite("microdomain_id", &circuit::astrocyte_t::microdomain_id)
        .def_readwrite("soma_center", &circuit::astrocyte_t::soma_center)
        .def_readwrite("soma_radius", &circuit::astrocyte_t::soma_radius)
        .def_readwrite("name", &circuit::astrocyte_t::name)
        .def_readwrite("mtype", &circuit::astrocyte_t::mtype)
        .def_readwrite("morphology_filename", &circuit::astrocyte_t::morphology_filename)
        .def_readwrite("synapses_idx", &circuit::astrocyte_t::synapses_idx)
        .def_readwrite("neurons_idx", &circuit::astrocyte_t::neurons_idx);

    py::class_<circuit::microdomain_t>(m, "MicroDomain")
        .def(py::init<>())
        .def(py::init(&circuit::microdomain_t::create), "microdomain_id"_a = 0,
             "astrocyte_id"_a = 0, "neighbors"_a = py::array_t<uint32_t>(0),
             "vertex_coordinates"_a = py::array_t<float>(0),
             "triangles"_a = py::array_t<uint32_t>(0),
             "centroid"_a = circuit::float_point_t({0., 0., 0.}), "area"_a = 0., "volume"_a = 0.,
             "mesh_filename"_a = "", "neurons_idx"_a = py::array_t<uint32_t>(0),
             "synapses_idx"_a = py::array_t<uint32_t>(0))
        .def("serialize", &basalt::serialize<circuit::microdomain_t>)
        .def("deserialize", &basalt::deserialize<circuit::microdomain_t>)

        .def_readwrite("microdomain_id", &circuit::microdomain_t::microdomain_id)
        .def_readwrite("astrocyte_id", &circuit::microdomain_t::astrocyte_id)
        .def_readwrite("neighbors", &circuit::microdomain_t::neighbors)
        .def_readwrite("vertex_coordinates", &circuit::microdomain_t::vertex_coordinates)
        .def_readwrite("triangles", &circuit::microdomain_t::triangles)
        .def_readwrite("centroid", &circuit::microdomain_t::centroid)
        .def_readwrite("area", &circuit::microdomain_t::area)
        .def_readwrite("volume", &circuit::microdomain_t::volume)
        .def_readwrite("mesh_filename", &circuit::microdomain_t::mesh_filename)
        .def_readwrite("neurons_idx", &circuit::microdomain_t::neurons_idx)
        .def_readwrite("synapses_idx", &circuit::microdomain_t::synapses_idx);

    py::class_<circuit::segment_t>(m, "Segment")
        .def(py::init<>())
        .def(py::init<>(&circuit::segment_t::create), "section_id"_a = 0, "segment_id"_a = 0,
             "type"_a = 0, "x1"_a = 0., "y1"_a = 0., "z1"_a = 0., "r1"_a = 0., "x2"_a = 0.,
             "y2"_a = 0., "z2"_a = 0., "r2"_a = 0.)
        .def("serialize", &basalt::serialize<circuit::segment_t>)
        .def("deserialize", &basalt::deserialize<circuit::segment_t>)
        .def_readwrite("section_id", &circuit::segment_t::section_id)
        .def_readwrite("segment_id", &circuit::segment_t::segment_id)
        .def_readwrite("type", &circuit::segment_t::type)
        .def_readwrite("x1", &circuit::segment_t::x1)
        .def_readwrite("y1", &circuit::segment_t::y1)
        .def_readwrite("z1", &circuit::segment_t::z1)
        .def_readwrite("r1", &circuit::segment_t::r1)
        .def_readwrite("x2", &circuit::segment_t::x2)
        .def_readwrite("y2", &circuit::segment_t::y2)
        .def_readwrite("z2", &circuit::segment_t::z2)
        .def_readwrite("r2", &circuit::segment_t::r2);

    py::class_<circuit::edge_astrocyte_segment_t>(m, "EdgeAstrocyteSegment")
        .def(py::init<>())
        .def(py::init<>(&circuit::edge_astrocyte_segment_t::create),
             "astrocyte"_a = circuit::float_point_t({0., 0., 0.}),
             "vasculature"_a = circuit::float_point_t({0., 0., 0.}))
        .def("serialize", &basalt::serialize<circuit::edge_astrocyte_segment_t>)
        .def("deserialize", &basalt::deserialize<circuit::edge_astrocyte_segment_t>)
        .def_readwrite("astrocyte", &circuit::edge_astrocyte_segment_t::astrocyte)
        .def_readwrite("vasculature", &circuit::edge_astrocyte_segment_t::vasculature);

    m.def("make_id", &basalt::make_id, "node_id_t constructor helper function");

    py::class_<basalt::status_t>(m, "Status")
        .def_readonly("code", &basalt::status_t::code)
        .def_readonly("message", &basalt::status_t::message)
        .def("raise_on_error", &basalt::status_t::raise_on_error);

    py::class_<basalt::network_t>(m, "Network")
        .def(py::init<const std::string&>())
        .def_property_readonly("nodes", &basalt::network_t::nodes)
        .def_property_readonly("connections", &basalt::network_t::connections)
        .def("commit", &basalt::network_t::commit)
        .def("statistics", &basalt::network_t::statistics);

    py::class_<basalt::connections_t>(m, "Connections")
        .def("insert",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node1,
                const basalt::node_uid_t& node2, bool commit) {
                 const auto status = connections.insert(node1, node2, commit);
                 status.raise_on_error();
             },
             "Create an edge between 2 existing nodes", "node1"_a, "node2"_a, "commit"_a = false)

        .def("insert",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node1,
                const basalt::node_uid_t& node2, py::array_t<char> data, bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'data' must be one");
                 }
                 const auto status = connections.insert(node1, node2, data.data(),
                                                        static_cast<std::size_t>(data.size()),
                                                        commit);
                 status.raise_on_error();
             },
             "Create an edge with a payload between 2 existing nodes", "node1"_a, "node2"_a,
             "data"_a, "commit"_a = false)

        .def("insert",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node,
                const basalt::node_t type, py::array_t<uint64_t> nodes, bool commit,
                bool create_nodes) {
                 if (nodes.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions of array 'nodes' must be one");
                 }
                 connections
                     .insert(node, type, nodes.data(), static_cast<std::size_t>(nodes.size()),
                             create_nodes, commit)
                     .raise_on_error();
             },
             "Create an edge between a node and a list of other nodes", "node"_a, "type"_a,
             "nodes"_a, "commit"_a = false, "create_nodes"_a = false)

        .def("has",
             [](const basalt::connections_t& connections, const basalt::node_uid_t& node1,
                const basalt::node_uid_t& node2) {
                 bool result;
                 connections.has(node1, node2, result).raise_on_error();
                 return result;
             },
             "Check connectivity between 2 nodes", "node1"_a, "node2"_a)

        .def("get",
             [](const basalt::connections_t& connections, const basalt::node_uid_t& node) {
                 basalt::node_uids_t eax;
                 connections.get(node, eax).raise_on_error();
                 return eax;
             },
             "get nodes connected to one node", "node"_a)

        .def("get",
             [](const basalt::connections_t& connections, const basalt::node_uid_t& node,
                basalt::node_t filter) {
                 basalt::node_uids_t eax;
                 connections.get(node, filter, eax).raise_on_error();
                 return eax;
             },
             "get nodes of a specific type connected to one node", "node"_a, "filter"_a)

        .def("erase",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node1,
                const basalt::node_uid_t& node2,
                bool commit = false) { connections.erase(node1, node2, commit).raise_on_error(); },
             "remove connection between 2 nodes", "node1"_a, "node2"_a, "commit"_a = false)

        .def("erase",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node,
                bool commit = false) {
                 std::size_t removed;
                 connections.erase(node, removed, commit).raise_on_error();
                 return removed;
             },
             "remove all connections of a node", "node"_a, "commit"_a = false)

        .def("erase",
             [](basalt::connections_t& connections, const basalt::node_uid_t& node,
                basalt::node_t filter, bool commit = false) {
                 std::size_t removed;
                 connections.erase(node, filter, removed, commit).raise_on_error();
                 return removed;
             },
             "remove all connections of a node", "node"_a, "filter"_a, "commit"_a = false);

    py::class_<basalt::nodes_t>(m, "Nodes")
        .def("__iter__",
             [](const basalt::nodes_t& nodes) {
                 return py::make_iterator(nodes.begin(), nodes.end());
             },
             py::keep_alive<0, 1>())

        .def("insert",
             [](basalt::nodes_t& nodes, basalt::node_t type, basalt::node_id_t id,
                bool commit = false) {
                 basalt::node_uid_t uid;
                 const auto status = nodes.insert(type, id, uid, commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a node in the graph", "type"_a, "id"_a, "commit"_a = false)

        .def("insert",
             [](basalt::nodes_t& nodes, basalt::node_t type, basalt::node_id_t id,
                py::array_t<char> data, bool commit = false) {
                 if (data.ndim() != 1) {
                     throw std::runtime_error("Number of dimensions must be one");
                 }
                 basalt::node_uid_t uid;
                 const auto status = nodes.insert(type, id, data.data(),
                                                  static_cast<std::size_t>(data.size()), uid,
                                                  commit);
                 status.raise_on_error();
                 return uid;
             },
             "insert a node in the graph", "node"_a, "id"_a, "data"_a, "commit"_a = false)

        .def("get",
             [](basalt::nodes_t& nodes, basalt::node_uid_t node) -> py::object {
                 std::string data;
                 auto const& status = nodes.get(node, &data);
                 if (status.code == basalt::status_t::missing_node_code) {
                     return py::none();
                 }
                 status.raise_on_error();
                 return std::move(basalt::to_py_array(data));
             },
             "Retrieve a node from the graph", "node"_a)

        .def("has",
             [](basalt::nodes_t& nodes, basalt::node_t type, basalt::node_id_t id) {
                 bool result = false;
                 const auto uid = basalt::make_id(type, id);
                 auto const status = nodes.has(uid, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a node in the graph", "type"_a, "id"_a)

        .def("has",
             [](basalt::nodes_t& nodes, basalt::node_uid_t node) {
                 bool result = false;
                 auto const status = nodes.has(node, result);
                 status.raise_on_error();
                 return result;
             },
             "Check presence of a node in the graph", "node"_a)

        .def("erase",
             [](basalt::nodes_t& nodes, basalt::node_uid_t node, bool commit) {
                 auto const status = nodes.erase(node, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a node from the graph", "node"_a, "commit"_a = false)

        .def("erase",
             [](basalt::nodes_t& nodes, basalt::node_t type, basalt::node_id_t id, bool commit) {
                 auto const node = basalt::make_id(type, id);
                 auto const status = nodes.erase(node, commit);
                 status.raise_on_error();
                 return status;
             },
             "Remove a node from the graph", "type"_a, "id"_a, "commit"_a = false);
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif  // defined(__clang__)
