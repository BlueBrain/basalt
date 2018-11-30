/**
  * \file src/basalt/circuit_payloads.cpp
  * Provides definition of classes used as payload for node
  * and edges of biological circuit made of neurons, synapses,
  * astrocytes, and segments.
*/
#include "circuit_payloads.hpp"
#include "py_helpers.hpp"

namespace basalt {
namespace circuit {

// neuron_t

std::unique_ptr<neuron_t>
neuron_t::create(uint32_t gid, pybind11::array_t<uint32_t>& astro_idx,
                 pybind11::array_t<uint32_t>& syn_idx) {
    std::unique_ptr<neuron_t> eax(new neuron_t);
    eax->gid = gid;
    fill_vector(astro_idx, eax->astro_idx);
    fill_vector(syn_idx, eax->syn_idx);
    return eax;
}

pybind11::array_t<char> neuron_t::serialize() const {
    std::ostringstream oss;
    oss << gid << ' ';
    serialize_vector(oss, astro_idx);
    serialize_vector(oss, syn_idx);
    return to_py_array(oss);
}

void neuron_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> gid;
    deserialize_vector(iss, astro_idx);
    deserialize_vector(iss, syn_idx);
}

// synapse_t

std::unique_ptr<synapse_t>
synapse_t::create(uint32_t pre_gid, uint32_t post_gid, uint32_t nrn_idx,
                  pybind11::array_t<uint32_t>& astro_idx, bool is_excitatory,
                  const float_point_t& pre, const float_point_t& post,
                  const std::string& mesh_filename,
                  const std::string& skeleton_filename, float psd_area) {
    std::unique_ptr<synapse_t> eax(new synapse_t);
    eax->pre_gid = pre_gid;
    eax->post_gid = post_gid;
    eax->nrn_idx = nrn_idx;
    fill_vector(astro_idx, eax->astro_idx);
    eax->is_excitatory = is_excitatory;
    eax->pre = pre;
    eax->post = post;
    eax->mesh_filename = mesh_filename;
    eax->skeleton_filename = skeleton_filename;
    eax->psd_area = psd_area;
    return eax;
}

pybind11::array_t<char> synapse_t::serialize() const {
    std::ostringstream oss;
    oss << pre_gid << ' ' << post_gid << ' ' << nrn_idx << ' ';
    serialize_vector(oss, astro_idx);
    oss << is_excitatory << ' ' << pre << ' ' << post << ' ' << mesh_filename
        << ' ' << skeleton_filename << ' ' << psd_area;
    return to_py_array(oss);
}

void synapse_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> pre_gid >> post_gid >> nrn_idx;
    deserialize_vector(iss, astro_idx);
    iss >> is_excitatory >> pre >> post >> mesh_filename >> skeleton_filename >>
        psd_area;
}

// astrocyte_t

std::unique_ptr<astrocyte_t>
astrocyte_t::create(uint32_t astrocyte_id, uint32_t microdomain_id,
                    float soma_center_x, float soma_center_y,
                    float soma_center_z, float soma_radius,
                    const std::string& name, const std::string& mtype,
                    const std::string& morphology_filename,
                    pybind11::array_t<uint32_t>& synapses_idx,
                    pybind11::array_t<uint32_t>& neurons_idx) {
    std::unique_ptr<astrocyte_t> eax(new astrocyte_t);
    eax->astrocyte_id = astrocyte_id;
    eax->microdomain_id = microdomain_id;
    eax->soma_center_x = soma_center_x;
    eax->soma_center_y = soma_center_y;
    eax->soma_center_z = soma_center_z;
    eax->soma_radius = soma_radius;
    eax->name = name;
    eax->mtype = mtype;
    eax->morphology_filename = morphology_filename;
    fill_vector(synapses_idx, eax->synapses_idx);
    fill_vector(neurons_idx, eax->neurons_idx);
    return eax;
}

pybind11::array astrocyte_t::serialize() const {
    std::ostringstream oss;
    oss << astrocyte_id << ' ' << microdomain_id << ' ' << soma_center_x << ' '
        << soma_center_y << ' ' << soma_center_z << ' ' << soma_radius << ' '
        << name << ' ' << mtype << ' ' << morphology_filename << ' ';
    serialize_vector(oss, synapses_idx);
    serialize_vector(oss, neurons_idx);
    return to_py_array(oss);
}

void astrocyte_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> astrocyte_id >> microdomain_id >> soma_center_x >> soma_center_y >>
        soma_center_z >> soma_radius >> name >> mtype >> morphology_filename;
    deserialize_vector(iss, synapses_idx);
    deserialize_vector(iss, neurons_idx);
}

// microdomain_t

std::unique_ptr<microdomain_t>
microdomain_t::create(uint32_t microdomain_id, uint32_t astrocyte_id,
                      pybind11::array_t<uint32_t>& neighbors,
                      pybind11::array_t<float>& vertex_coordinates,
                      pybind11::array_t<uint32_t>& triangles,
                      const float_point_t& centroid, double area, double volume,
                      const std::string& mesh_filename,
                      pybind11::array_t<uint32_t>& neurons_idx,
                      pybind11::array_t<uint32_t>& synapses_idx) {
    std::unique_ptr<microdomain_t> eax(new microdomain_t);

    eax->microdomain_id = microdomain_id;
    eax->astrocyte_id = astrocyte_id;
    fill_vector(neighbors, eax->neighbors);
    fill_vector(vertex_coordinates, eax->vertex_coordinates);
    fill_vector(triangles, eax->triangles);
    eax->centroid = centroid;
    eax->area = area;
    eax->volume = volume;
    eax->mesh_filename = mesh_filename;
    fill_vector(neurons_idx, eax->neurons_idx);
    fill_vector(synapses_idx, eax->synapses_idx);
    return eax;
}

pybind11::array microdomain_t::serialize() const {
    std::ostringstream oss;
    oss << microdomain_id << ' ' << astrocyte_id << ' ';
    serialize_vector(oss, neighbors);
    serialize_vector(oss, vertex_coordinates);
    serialize_vector(oss, triangles);

    oss << centroid << ' ' << area << ' ' << volume << ' ' << mesh_filename
        << ' ';
    serialize_vector(oss, neurons_idx);
    serialize_vector(oss, synapses_idx);

    return to_py_array(oss);
}

void microdomain_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> microdomain_id >> astrocyte_id;
    deserialize_vector(iss, neighbors);
    deserialize_vector(iss, vertex_coordinates);
    deserialize_vector(iss, triangles);
    iss >> centroid >> area >> volume >> mesh_filename;
    deserialize_vector(iss, neurons_idx);
    deserialize_vector(iss, synapses_idx);
}

// segment_t

std::unique_ptr<segment_t> segment_t::create(uint32_t section_id,
                                             uint32_t segment_id, uint8_t type,
                                             float x1, float y1, float z1,
                                             float r1, float x2, float y2,
                                             float z2, float r2) {
    std::unique_ptr<segment_t> eax(new segment_t);
    eax->section_id = section_id;
    eax->segment_id = segment_id;
    eax->type = type;
    eax->x1 = x1;
    eax->y1 = y1;
    eax->z1 = z1;
    eax->r1 = r1;
    eax->x2 = x2;
    eax->y2 = y2;
    eax->z2 = z2;
    eax->r2 = r2;
    return eax;
}

pybind11::array segment_t::serialize() const {
    std::ostringstream oss;
    oss << section_id << ' ' << segment_id << ' ' << type << ' ' << x1 << ' '
        << y1 << ' ' << z1 << ' ' << r1 << ' ' << x2 << ' ' << y2 << ' ' << z2
        << ' ' << r2;
    return to_py_array(oss);
}

void segment_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> section_id >> segment_id >> type >> x1 >> y1 >> z1 >> r1 >> x2 >>
        y2 >> z2 >> r2;
}

// edge_astrocyte_segment_t

std::unique_ptr<edge_astrocyte_segment_t>
edge_astrocyte_segment_t::create(const float_point_t& astrocyte,
                                 const float_point_t& vasculature) {

    std::unique_ptr<edge_astrocyte_segment_t> eax(new edge_astrocyte_segment_t);
    eax->astrocyte = astrocyte;
    eax->vasculature = vasculature;
    return eax;
}

pybind11::array edge_astrocyte_segment_t::serialize() const {
    std::ostringstream oss;
    oss << astrocyte << ' ' << vasculature;
    return to_py_array(oss);
}

void edge_astrocyte_segment_t::deserialize(pybind11::array_t<char>& data) {
    std::istringstream iss;
    from_py_array(data, iss);
    iss >> astrocyte >> vasculature;
}

} // namespace circuit
} // namespace basalt
