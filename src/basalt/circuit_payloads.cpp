/**
 * \file src/basalt/circuit_payloads.cpp
 * Provides definition of classes used as payload for node
 * and edges of biological circuit made of neurons, synapses,
 * astrocytes, and segments.
 */

#include "circuit_payloads.hpp"
#include "basalt/settings.hpp"
#include "py_helpers.hpp"

namespace circuit {

// neuron_t

neuron_t::neuron_t(uint32_t gid, int_vector_t& astro_idx, int_vector_t& syn_idx)
    : gid(gid)
    , astro_idx(std::move(astro_idx))
    , syn_idx(std::move(syn_idx)) {}

std::unique_ptr<neuron_t> neuron_t::create(uint32_t gid, pybind11::array_t<uint32_t>& astro_idx,
                                           pybind11::array_t<uint32_t>& syn_idx) {
    int_vector_t astro_idx_vector;
    int_vector_t syn_idx_vector;
    basalt::fill_vector(astro_idx, astro_idx_vector);
    basalt::fill_vector(syn_idx, syn_idx_vector);
    return std::unique_ptr<neuron_t>(new neuron_t(gid, astro_idx_vector, syn_idx_vector));
}

pybind11::array_t<char> neuron_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << gid << ' ';
    basalt::serialize_vector(oss, astro_idx);
    basalt::serialize_vector(oss, syn_idx);
    return basalt::to_py_array(oss);
}

void neuron_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> gid;
    basalt::deserialize_vector(iss, astro_idx);
    basalt::deserialize_vector(iss, syn_idx);
}

// synapse_t

synapse_t::synapse_t(uint32_t pre_gid, uint32_t post_gid, uint32_t nrn_idx, int_vector_t& astro_idx,
                     bool is_excitatory, const float_point_t& pre, const float_point_t& post,
                     const std::string& mesh_filename, const std::string& skeleton_filename,
                     float psd_area)
    : pre_gid(pre_gid)
    , post_gid(post_gid)
    , nrn_idx(nrn_idx)
    , astro_idx(std::move(astro_idx))
    , is_excitatory(is_excitatory)
    , pre(std::move(pre))
    , post(std::move(post))
    , mesh_filename(mesh_filename)
    , skeleton_filename(skeleton_filename)
    , psd_area(psd_area) {}

std::unique_ptr<synapse_t> synapse_t::create(uint32_t pre_gid, uint32_t post_gid, uint32_t nrn_idx,
                                             pybind11::array_t<uint32_t>& astro_idx,
                                             bool is_excitatory, const float_point_t& pre,
                                             const float_point_t& post,
                                             const std::string& mesh_filename,
                                             const std::string& skeleton_filename, float psd_area) {
    int_vector_t astro_idx_vector;
    basalt::fill_vector(astro_idx, astro_idx_vector);
    return std::unique_ptr<synapse_t>(new synapse_t(pre_gid, post_gid, nrn_idx, astro_idx_vector,
                                                    is_excitatory, pre, post, mesh_filename,
                                                    skeleton_filename, psd_area));
}

pybind11::array_t<char> synapse_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << pre_gid << ' ' << post_gid << ' ' << nrn_idx << ' ';
    basalt::serialize_vector(oss, astro_idx);
    oss << is_excitatory << ' ' << pre << ' ' << post << ' ' << mesh_filename << ' '
        << skeleton_filename << ' ' << psd_area;
    return basalt::to_py_array(oss);
}

void synapse_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> pre_gid >> post_gid >> nrn_idx;
    basalt::deserialize_vector(iss, astro_idx);
    iss >> is_excitatory >> pre >> post >> mesh_filename >> skeleton_filename >> psd_area;
}

// astrocyte_t

astrocyte_t::astrocyte_t(uint32_t astrocyte_id, uint32_t microdomain_id,
                         const float_point_t& soma_center, float soma_radius,
                         const std::string& name, const std::string& mtype,
                         const std::string& morphology_filename, int_vector_t& synapses_idx,
                         int_vector_t& neurons_idx)
    : astrocyte_id(astrocyte_id)
    , microdomain_id(microdomain_id)
    , soma_center(soma_center)
    , soma_radius(soma_radius)
    , name(name)
    , mtype(mtype)
    , morphology_filename(morphology_filename)
    , synapses_idx(std::move(synapses_idx))
    , neurons_idx(std::move(neurons_idx)) {}

std::unique_ptr<astrocyte_t> astrocyte_t::create(uint32_t astrocyte_id, uint32_t microdomain_id,
                                                 const float_point_t& soma_center,
                                                 float soma_radius, const std::string& name,
                                                 const std::string& mtype,
                                                 const std::string& morphology_filename,
                                                 pybind11::array_t<uint32_t>& synapses_idx,
                                                 pybind11::array_t<uint32_t>& neurons_idx) {
    int_vector_t synapses_idx_vector;
    int_vector_t neurons_idx_vector;
    basalt::fill_vector(synapses_idx, synapses_idx_vector);
    basalt::fill_vector(neurons_idx, neurons_idx_vector);
    return std::unique_ptr<astrocyte_t>(
        new astrocyte_t(astrocyte_id, microdomain_id, soma_center, soma_radius, name, mtype,
                        morphology_filename, synapses_idx_vector, neurons_idx_vector));
}

pybind11::array astrocyte_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << astrocyte_id << ' ' << microdomain_id << ' ' << soma_center << ' ' << soma_radius << ' '
        << name << ' ' << mtype << ' ' << morphology_filename << ' ';
    basalt::serialize_vector(oss, synapses_idx);
    basalt::serialize_vector(oss, neurons_idx);
    return basalt::to_py_array(oss);
}

void astrocyte_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> astrocyte_id >> microdomain_id >> soma_center >> soma_radius >> name >> mtype >>
        morphology_filename;
    basalt::deserialize_vector(iss, synapses_idx);
    basalt::deserialize_vector(iss, neurons_idx);
}

// microdomain_t

microdomain_t::microdomain_t(uint32_t microdomain_id, uint32_t astrocyte_id,
                             int_vector_t& neighbors, vertex_coordinates_t& vertex_coordinates,
                             triangles_t& triangles, const float_point_t& centroid, double area,
                             double volume, const std::string& mesh_filename,
                             int_vector_t& neurons_idx, int_vector_t& synapses_idx)
    : microdomain_id(microdomain_id)
    , astrocyte_id(astrocyte_id)
    , neighbors(std::move(neighbors))
    , vertex_coordinates(std::move(vertex_coordinates))
    , triangles(std::move(triangles))
    , centroid(centroid)
    , area(area)
    , volume(volume)
    , mesh_filename(mesh_filename)
    , neurons_idx(std::move(neurons_idx))
    , synapses_idx(std::move(synapses_idx)) {}

std::unique_ptr<microdomain_t> microdomain_t::create(
    uint32_t microdomain_id, uint32_t astrocyte_id, pybind11::array_t<uint32_t>& neighbors,
    pybind11::array_t<float>& vertex_coordinates, pybind11::array_t<uint32_t>& triangles,
    const float_point_t& centroid, double area, double volume, const std::string& mesh_filename,
    pybind11::array_t<uint32_t>& neurons_idx, pybind11::array_t<uint32_t>& synapses_idx) {
    int_vector_t neighbors_vector;
    vertex_coordinates_t vertex_coordinates_vector;
    triangles_t triangles_vector;
    int_vector_t neurons_idx_vector;
    int_vector_t synapses_idx_vector;
    basalt::fill_vector(neighbors, neighbors_vector);
    basalt::fill_vector(vertex_coordinates, vertex_coordinates_vector);
    basalt::fill_vector(triangles, triangles_vector);
    basalt::fill_vector(neurons_idx, neurons_idx_vector);
    basalt::fill_vector(synapses_idx, synapses_idx_vector);
    return std::unique_ptr<microdomain_t>(
        new microdomain_t(microdomain_id, astrocyte_id, neighbors_vector, vertex_coordinates_vector,
                          triangles_vector, centroid, area, volume, mesh_filename,
                          neurons_idx_vector, synapses_idx_vector));
}

pybind11::array microdomain_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << microdomain_id << ' ' << astrocyte_id << ' ';
    basalt::serialize_vector(oss, neighbors);
    basalt::serialize_vector(oss, vertex_coordinates);
    basalt::serialize_vector(oss, triangles);

    oss << centroid << ' ' << area << ' ' << volume << ' ' << mesh_filename << ' ';
    basalt::serialize_vector(oss, neurons_idx);
    basalt::serialize_vector(oss, synapses_idx);

    return basalt::to_py_array(oss);
}

void microdomain_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> microdomain_id >> astrocyte_id;
    basalt::deserialize_vector(iss, neighbors);
    basalt::deserialize_vector(iss, vertex_coordinates);
    basalt::deserialize_vector(iss, triangles);
    iss >> centroid >> area >> volume >> mesh_filename;
    basalt::deserialize_vector(iss, neurons_idx);
    basalt::deserialize_vector(iss, synapses_idx);
}

// segment_t

segment_t::segment_t(uint32_t section_id, uint32_t segment_id, uint8_t type, float x1, float y1,
                     float z1, float r1, float x2, float y2, float z2, float r2)
    : section_id(section_id)
    , segment_id(segment_id)
    , type(type)
    , x1(x1)
    , y1(y1)
    , z1(z1)
    , r1(r1)
    , x2(x2)
    , y2(y2)
    , z2(z2)
    , r2(r2) {}

std::unique_ptr<segment_t> segment_t::create(uint32_t section_id, uint32_t segment_id, uint8_t type,
                                             float x1, float y1, float z1, float r1, float x2,
                                             float y2, float z2, float r2) {
    return std::unique_ptr<segment_t>(
        new segment_t(section_id, segment_id, type, x1, y1, z1, r1, x2, y2, z2, r2));
}

pybind11::array segment_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << section_id << ' ' << segment_id << ' ' << type << ' ' << x1 << ' ' << y1 << ' ' << z1
        << ' ' << r1 << ' ' << x2 << ' ' << y2 << ' ' << z2 << ' ' << r2;
    return basalt::to_py_array(oss);
}

void segment_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> section_id >> segment_id >> type >> x1 >> y1 >> z1 >> r1 >> x2 >> y2 >> z2 >> r2;
}

// edge_astrocyte_segment_t

edge_astrocyte_segment_t::edge_astrocyte_segment_t(const float_point_t& astrocyte,
                                                   const float_point_t& vasculature)
    : astrocyte(astrocyte)
    , vasculature(vasculature) {}

std::unique_ptr<edge_astrocyte_segment_t> edge_astrocyte_segment_t::create(
    const float_point_t& astrocyte, const float_point_t& vasculature) {
    return std::unique_ptr<edge_astrocyte_segment_t>(
        new edge_astrocyte_segment_t(astrocyte, vasculature));
}

pybind11::array edge_astrocyte_segment_t::serialize_sstream() const {
    std::ostringstream oss;
    oss << astrocyte << ' ' << vasculature;
    return basalt::to_py_array(oss);
}

void edge_astrocyte_segment_t::deserialize_sstream(pybind11::array_t<char>& data) {
    basalt::imemstream iss(data);
    iss >> astrocyte >> vasculature;
}

}  // namespace circuit
