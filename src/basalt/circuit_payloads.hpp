/**
 * \file src/basalt/circuit_payloads.hpp
 * Provides declaration of classes used as payload for node
 * and edges of biological circuit made of neurons, synapses,
 * astrocytes, and segments.
 */

#ifndef BASALT_CIRCUIT_PAYLOADS_HPP
#define BASALT_CIRCUIT_PAYLOADS_HPP

#include "py_helpers.hpp"

namespace basalt {
namespace circuit {

using float_point_t = std::array<float, 3>;
using point_vector_t = std::vector<float_point_t>;
using int_point_t = std::array<uint32_t, 3>;
using int_vector_t = std::vector<uint32_t>;

/**
 * \name Node and edge payloads
 * \{
 */

/**
 * \brief Payload description of a node of type Neuron
 */
struct neuron_t {
    uint32_t gid;
    int_vector_t astro_idx;
    int_vector_t syn_idx;

    static std::unique_ptr<neuron_t>
    create(uint32_t gid, pybind11::array_t<uint32_t>& astro_idx,
           pybind11::array_t<uint32_t>& syn_idx);
    pybind11::array_t<char> serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 * \brief Payload description of a node of type Synapse
 */
struct synapse_t {
    uint32_t pre_gid;
    uint32_t post_gid;
    uint32_t nrn_idx;
    int_vector_t astro_idx;

    bool is_excitatory;
    float_point_t pre;
    float_point_t post;
    std::string mesh_filename;
    std::string skeleton_filename;

    float psd_area;

    static std::unique_ptr<synapse_t>
    create(uint32_t pre_gid, uint32_t post_gid, uint32_t nrn_idx,
           pybind11::array_t<uint32_t>& astro_idx, bool is_excitatory,
           const float_point_t& pre, const float_point_t& post,
           const std::string& mesh_filename,
           const std::string& skeleton_filename, float psd_area);
    pybind11::array_t<char> serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 * \brief Payload description of a node of type Astrocyte
 */
struct astrocyte_t {
    uint32_t astrocyte_id;
    uint32_t microdomain_id;

    float soma_center_x;
    float soma_center_y;
    float soma_center_z;

    float soma_radius;

    std::string name;
    std::string mtype;
    std::string morphology_filename;

    int_vector_t synapses_idx;
    int_vector_t neurons_idx;

    static std::unique_ptr<astrocyte_t>
    create(uint32_t astrocyte_id, uint32_t microdomain_id, float soma_center_x,
           float soma_center_y, float soma_center_z, float soma_radius,
           const std::string& name, const std::string& mtype,
           const std::string& morphology_filename,
           pybind11::array_t<uint32_t>& synapses_idx,
           pybind11::array_t<uint32_t>& neurons_idx);
    pybind11::array serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 * \brief Payload description of a node of type MicroDomain
 */
struct microdomain_t {
    uint32_t microdomain_id;
    uint32_t astrocyte_id;

    // potentially different than astrocyte actual neighbors
    // microdomains form a tesselation
    int_vector_t neighbors;

    // mesh data
    std::vector<std::array<float, 3>> vertex_coordinates;
    std::vector<std::array<uint32_t, 3>> triangles;

    // geometric centroid, not the same as the morphology soma center
    float_point_t centroid;

    double area;
    double volume;

    // unsure if this is really essential. Meshes are created from
    // vertex_coordinates and triangles
    std::string mesh_filename;

    // maybe it's better to access neurons from synapses indirectly?
    int_vector_t neurons_idx;
    int_vector_t synapses_idx;

    static std::unique_ptr<microdomain_t>
    create(uint32_t microdomain_id, uint32_t astrocyte_id,
           pybind11::array_t<uint32_t>& neighbors,
           pybind11::array_t<float>& vertex_coordinates,
           pybind11::array_t<uint32_t>& triangles,
           const float_point_t& centroid, double area, double volume,
           const std::string& mesh_filename,
           pybind11::array_t<uint32_t>& neurons_idx,
           pybind11::array_t<uint32_t>& synapses_idx);
    pybind11::array serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 * \brief Payload description of a node of type Segment
 */
struct segment_t {
    uint32_t section_id;
    uint32_t segment_id;
    uint8_t type;
    float x1;
    float y1;
    float z1;
    float r1;
    float x2;
    float y2;
    float z2;
    float r2;

    static std::unique_ptr<segment_t> create(uint32_t section_id,
                                             uint32_t segment_id, uint8_t type,
                                             float x1, float y1, float z1,
                                             float r1, float x2, float y2,
                                             float z2, float r2);
    pybind11::array serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 * \brief Payload description of an edge between an Astrocyte and a Segment
 */
struct edge_astrocyte_segment_t {
    // endfoot starting point on morphology
    float_point_t astrocyte;

    // endfoot ending point on vasculature surface
    float_point_t vasculature;

    static std::unique_ptr<edge_astrocyte_segment_t>
    create(const float_point_t& astrocyte, const float_point_t& vasculature);
    pybind11::array serialize() const;
    void deserialize(pybind11::array_t<char>& data);
};

/**
 *  \}
 */

} // namespace circuit
} // namespace basalt

#endif // BASALT_CIRCUIT_PAYLOADS_HPP
