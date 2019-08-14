/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
/**
 * \file src/basalt/circuit_payloads.cpp
 * Provides definition of classes used as payload for vertex
 * and edges of biological circuit made of neurons, synapses,
 * astrocytes, and segments.
 */

#include "circuit_payloads.hpp"
#include "basalt/settings.hpp"
#include "py_helpers.hpp"

namespace circuit {

// neuron_t

neuron_t::neuron_t(uint32_t t_gid, int_vector_t& t_astro_idx, int_vector_t& t_syn_idx)
    : gid(t_gid)
    , astro_idx(std::move(t_astro_idx))
    , syn_idx(std::move(t_syn_idx)) {}

std::unique_ptr<neuron_t> neuron_t::create(uint32_t gid,
                                           pybind11::array_t<uint32_t>& astro_idx,
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

bool neuron_t::operator==(const neuron_t& rhs) const {
    return gid == rhs.gid && astro_idx == rhs.astro_idx && syn_idx == rhs.syn_idx;
}
bool neuron_t::operator!=(const neuron_t& rhs) const {
    return !(rhs == *this);
}

// synapse_t

synapse_t::synapse_t(uint32_t t_pre_gid,
                     uint32_t t_post_gid,
                     uint32_t t_nrn_idx,
                     int_vector_t& t_astro_idx,
                     bool t_is_excitatory,
                     const float_point_t& t_pre,
                     const float_point_t& t_post,
                     std::string t_mesh_filename,
                     std::string t_skeleton_filename,
                     float t_psd_area)
    : pre_gid(t_pre_gid)
    , post_gid(t_post_gid)
    , nrn_idx(t_nrn_idx)
    , astro_idx(std::move(t_astro_idx))
    , is_excitatory(t_is_excitatory)
    , pre(t_pre)
    , post(t_post)
    , mesh_filename(std::move(t_mesh_filename))
    , skeleton_filename(std::move(t_skeleton_filename))
    , psd_area(t_psd_area) {}

std::unique_ptr<synapse_t> synapse_t::create(uint32_t pre_gid,
                                             uint32_t post_gid,
                                             uint32_t nrn_idx,
                                             pybind11::array_t<uint32_t>& astro_idx,
                                             bool is_excitatory,
                                             const float_point_t& pre,
                                             const float_point_t& post,
                                             const std::string& mesh_filename,
                                             const std::string& skeleton_filename,
                                             float psd_area) {
    int_vector_t astro_idx_vector;
    basalt::fill_vector(astro_idx, astro_idx_vector);
    return std::unique_ptr<synapse_t>(new synapse_t(pre_gid,
                                                    post_gid,
                                                    nrn_idx,
                                                    astro_idx_vector,
                                                    is_excitatory,
                                                    pre,
                                                    post,
                                                    mesh_filename,
                                                    skeleton_filename,
                                                    psd_area));
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

bool synapse_t::operator==(const synapse_t& rhs) const {
    return pre_gid == rhs.pre_gid && post_gid == rhs.post_gid && nrn_idx == rhs.nrn_idx &&
           astro_idx == rhs.astro_idx && is_excitatory == rhs.is_excitatory && pre == rhs.pre &&
           post == rhs.post && mesh_filename == rhs.mesh_filename &&
           skeleton_filename == rhs.skeleton_filename && psd_area == rhs.psd_area;
}

bool synapse_t::operator!=(const synapse_t& rhs) const {
    return !(rhs == *this);
}

// astrocyte_t

astrocyte_t::astrocyte_t(uint32_t t_astrocyte_id,
                         uint32_t t_microdomain_id,
                         const float_point_t& t_soma_center,
                         float t_soma_radius,
                         std::string t_name,
                         std::string t_mtype,
                         std::string t_morphology_filename,
                         int_vector_t& t_synapses_idx,
                         int_vector_t& t_neurons_idx)
    : astrocyte_id(t_astrocyte_id)
    , microdomain_id(t_microdomain_id)
    , soma_center(t_soma_center)
    , soma_radius(t_soma_radius)
    , name(std::move(t_name))
    , mtype(std::move(t_mtype))
    , morphology_filename(std::move(t_morphology_filename))
    , synapses_idx(std::move(t_synapses_idx))
    , neurons_idx(std::move(t_neurons_idx)) {}

std::unique_ptr<astrocyte_t> astrocyte_t::create(uint32_t astrocyte_id,
                                                 uint32_t microdomain_id,
                                                 const float_point_t& soma_center,
                                                 float soma_radius,
                                                 const std::string& name,
                                                 const std::string& mtype,
                                                 const std::string& morphology_filename,
                                                 pybind11::array_t<uint32_t>& synapses_idx,
                                                 pybind11::array_t<uint32_t>& neurons_idx) {
    int_vector_t synapses_idx_vector;
    int_vector_t neurons_idx_vector;
    basalt::fill_vector(synapses_idx, synapses_idx_vector);
    basalt::fill_vector(neurons_idx, neurons_idx_vector);
    return std::unique_ptr<astrocyte_t>(new astrocyte_t(astrocyte_id,
                                                        microdomain_id,
                                                        soma_center,
                                                        soma_radius,
                                                        name,
                                                        mtype,
                                                        morphology_filename,
                                                        synapses_idx_vector,
                                                        neurons_idx_vector));
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

bool astrocyte_t::operator==(const astrocyte_t& rhs) const {
    return astrocyte_id == rhs.astrocyte_id && microdomain_id == rhs.microdomain_id &&
           soma_center == rhs.soma_center && soma_radius == rhs.soma_radius && name == rhs.name &&
           mtype == rhs.mtype && morphology_filename == rhs.morphology_filename &&
           synapses_idx == rhs.synapses_idx && neurons_idx == rhs.neurons_idx;
}

bool astrocyte_t::operator!=(const astrocyte_t& rhs) const {
    return !(rhs == *this);
}

// microdomain_t

microdomain_t::microdomain_t(uint32_t t_microdomain_id,
                             uint32_t t_astrocyte_id,
                             int_vector_t& t_neighbors,
                             vertex_coordinates_t& t_vertex_coordinates,
                             triangles_t& t_triangles,
                             const float_point_t& t_centroid,
                             double t_area,
                             double t_volume,
                             std::string t_mesh_filename,
                             int_vector_t& t_neurons_idx,
                             int_vector_t& t_synapses_idx)
    : microdomain_id(t_microdomain_id)
    , astrocyte_id(t_astrocyte_id)
    , neighbors(std::move(t_neighbors))
    , vertex_coordinates(std::move(t_vertex_coordinates))
    , triangles(std::move(t_triangles))
    , centroid(t_centroid)
    , area(t_area)
    , volume(t_volume)
    , mesh_filename(std::move(t_mesh_filename))
    , neurons_idx(std::move(t_neurons_idx))
    , synapses_idx(std::move(t_synapses_idx)) {}

std::unique_ptr<microdomain_t> microdomain_t::create(uint32_t microdomain_id,
                                                     uint32_t astrocyte_id,
                                                     pybind11::array_t<uint32_t>& neighbors,
                                                     pybind11::array_t<float>& vertex_coordinates,
                                                     pybind11::array_t<uint32_t>& triangles,
                                                     const float_point_t& centroid,
                                                     double area,
                                                     double volume,
                                                     const std::string& mesh_filename,
                                                     pybind11::array_t<uint32_t>& neurons_idx,
                                                     pybind11::array_t<uint32_t>& synapses_idx) {
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
    return std::unique_ptr<microdomain_t>(new microdomain_t(microdomain_id,
                                                            astrocyte_id,
                                                            neighbors_vector,
                                                            vertex_coordinates_vector,
                                                            triangles_vector,
                                                            centroid,
                                                            area,
                                                            volume,
                                                            mesh_filename,
                                                            neurons_idx_vector,
                                                            synapses_idx_vector));
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

bool microdomain_t::operator==(const microdomain_t& rhs) const {
    return microdomain_id == rhs.microdomain_id && astrocyte_id == rhs.astrocyte_id &&
           neighbors == rhs.neighbors && vertex_coordinates == rhs.vertex_coordinates &&
           triangles == rhs.triangles && centroid == rhs.centroid && area == rhs.area &&
           volume == rhs.volume && mesh_filename == rhs.mesh_filename &&
           neurons_idx == rhs.neurons_idx && synapses_idx == rhs.synapses_idx;
}

bool microdomain_t::operator!=(const microdomain_t& rhs) const {
    return !(rhs == *this);
}

// segment_t

segment_t::segment_t(uint32_t t_section_id,
                     uint32_t t_segment_id,
                     uint8_t t_type,
                     float t_x1,
                     float t_y1,
                     float t_z1,
                     float t_r1,
                     float t_x2,
                     float t_y2,
                     float t_z2,
                     float t_r2)
    : section_id(t_section_id)
    , segment_id(t_segment_id)
    , type(t_type)
    , x1(t_x1)
    , y1(t_y1)
    , z1(t_z1)
    , r1(t_r1)
    , x2(t_x2)
    , y2(t_y2)
    , z2(t_z2)
    , r2(t_r2) {}

std::unique_ptr<segment_t> segment_t::create(uint32_t section_id,
                                             uint32_t segment_id,
                                             uint8_t type,
                                             float x1,
                                             float y1,
                                             float z1,
                                             float r1,
                                             float x2,
                                             float y2,
                                             float z2,
                                             float r2) {
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
bool segment_t::operator!=(const segment_t& rhs) const {
    return !(rhs == *this);
}

bool segment_t::operator==(const segment_t& rhs) const {
    return section_id == rhs.section_id && segment_id == rhs.segment_id && type == rhs.type &&
           x1 == rhs.x1 && y1 == rhs.y1 && z1 == rhs.z1 && r1 == rhs.r1 && x2 == rhs.x2 &&
           y2 == rhs.y2 && z2 == rhs.z2 && r2 == rhs.r2;
}

// edge_astrocyte_segment_t

edge_astrocyte_segment_t::edge_astrocyte_segment_t(const float_point_t& t_astrocyte,
                                                   const float_point_t& t_vasculature)
    : astrocyte(t_astrocyte)
    , vasculature(t_vasculature) {}

std::unique_ptr<edge_astrocyte_segment_t> edge_astrocyte_segment_t::create(
    const float_point_t& astrocyte,
    const float_point_t& vasculature) {
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

bool edge_astrocyte_segment_t::operator==(const edge_astrocyte_segment_t& rhs) const {
    return astrocyte == rhs.astrocyte && vasculature == rhs.vasculature;
}

bool edge_astrocyte_segment_t::operator!=(const edge_astrocyte_segment_t& rhs) const {
    return !(rhs == *this);
}

}  // namespace circuit
