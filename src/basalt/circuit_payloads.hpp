#pragma once

/**
 * \file src/basalt/circuit_payloads.hpp
 * Provides declaration of classes used as payload for vertex
 * and edges of biological circuit made of neurons, synapses,
 * astrocytes, and segments.
 */

#include "py_helpers.hpp"

namespace circuit {

using float_point_t = std::array<float, 3>;
using point_vector_t = std::vector<float_point_t>;
using int_point_t = std::array<uint32_t, 3>;
using int_vector_t = std::vector<uint32_t>;

/**
 * \name Vertex and edge payloads
 * \{
 */

/**
 * \brief Payload description of a vertex of type Neuron
 */
struct neuron_t {
    neuron_t() = default;
    neuron_t(uint32_t gid, int_vector_t& astro_idx, int_vector_t& syn_idx);

    uint32_t gid;
    int_vector_t astro_idx;
    int_vector_t syn_idx;

    static std::unique_ptr<neuron_t> create(uint32_t gid,
                                            pybind11::array_t<uint32_t>& astro_idx,
                                            pybind11::array_t<uint32_t>& syn_idx);
    pybind11::array_t<char> serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a neuron_t
template <class Archive>
void serialize(Archive& ar, neuron_t& neuron) {
    ar(neuron.gid, neuron.astro_idx, neuron.syn_idx);
}

/**
 * \brief Payload description of a vertex of type Synapse
 */
struct synapse_t {
    synapse_t() = default;
    synapse_t(uint32_t pre_gid,
              uint32_t post_gid,
              uint32_t nrn_idx,
              int_vector_t& astro_idx,
              bool is_excitatory,
              const float_point_t& pre,
              const float_point_t& post,
              std::string mesh_filename,
              std::string skeleton_filename,
              float psd_area);

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

    static std::unique_ptr<synapse_t> create(uint32_t pre_gid,
                                             uint32_t post_gid,
                                             uint32_t nrn_idx,
                                             pybind11::array_t<uint32_t>& astro_idx,
                                             bool is_excitatory,
                                             const float_point_t& pre,
                                             const float_point_t& post,
                                             const std::string& mesh_filename,
                                             const std::string& skeleton_filename,
                                             float psd_area);
    pybind11::array_t<char> serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a synapse_t
template <class Archive>
void serialize(Archive& ar, synapse_t& s) {
    ar(s.pre_gid, s.post_gid, s.nrn_idx, s.astro_idx, s.is_excitatory, s.pre, s.post,
       s.mesh_filename, s.skeleton_filename, s.psd_area);
}

/**
 * \brief Payload description of a vertex of type Astrocyte
 */
struct astrocyte_t {
    astrocyte_t() = default;
    astrocyte_t(uint32_t astrocyte_id,
                uint32_t microdomain_id,
                const float_point_t& soma_center,
                float soma_radius,
                std::string name,
                std::string mtype,
                std::string morphology_filename,
                int_vector_t& synapses_idx,
                int_vector_t& neurons_idx);

    uint32_t astrocyte_id;
    uint32_t microdomain_id;

    float_point_t soma_center;
    float soma_radius;

    std::string name;
    std::string mtype;
    std::string morphology_filename;

    int_vector_t synapses_idx;
    int_vector_t neurons_idx;

    static std::unique_ptr<astrocyte_t> create(uint32_t astrocyte_id,
                                               uint32_t microdomain_id,
                                               const float_point_t& soma_center,
                                               float soma_radius,
                                               const std::string& name,
                                               const std::string& mtype,
                                               const std::string& morphology_filename,
                                               pybind11::array_t<uint32_t>& synapses_idx,
                                               pybind11::array_t<uint32_t>& neurons_idx);
    pybind11::array serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a astrocyte_t
template <class Archive>
void serialize(Archive& ar, astrocyte_t& a) {
    ar(a.astrocyte_id, a.microdomain_id, a.soma_center, a.soma_radius, a.name, a.mtype,
       a.morphology_filename, a.synapses_idx, a.neurons_idx);
}

/**
 * \brief Payload description of a vertex of type MicroDomain
 */
struct microdomain_t {
    using vertex_coordinates_t = std::vector<std::array<float, 3>>;
    using triangles_t = std::vector<std::array<uint32_t, 3>>;

    microdomain_t() = default;
    microdomain_t(uint32_t microdomain_id,
                  uint32_t astrocyte_id,
                  int_vector_t& neighbors,
                  vertex_coordinates_t& vertex_coordinates,
                  triangles_t& triangles,
                  const float_point_t& centroid,
                  double area,
                  double volume,
                  std::string mesh_filename,
                  int_vector_t& neurons_idx,
                  int_vector_t& synapses_idx);

    uint32_t microdomain_id;
    uint32_t astrocyte_id;

    // potentially different than astrocyte actual neighbors
    // microdomains form a tesselation
    int_vector_t neighbors;

    // mesh data
    vertex_coordinates_t vertex_coordinates;
    triangles_t triangles;

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

    static std::unique_ptr<microdomain_t> create(uint32_t microdomain_id,
                                                 uint32_t astrocyte_id,
                                                 pybind11::array_t<uint32_t>& neighbors,
                                                 pybind11::array_t<float>& vertex_coordinates,
                                                 pybind11::array_t<uint32_t>& triangles,
                                                 const float_point_t& centroid,
                                                 double area,
                                                 double volume,
                                                 const std::string& mesh_filename,
                                                 pybind11::array_t<uint32_t>& neurons_idx,
                                                 pybind11::array_t<uint32_t>& synapses_idx);
    pybind11::array serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a microdomain_t
template <class Archive>
void serialize(Archive& ar, microdomain_t& m) {
    ar(m.microdomain_id, m.astrocyte_id, m.neighbors, m.vertex_coordinates, m.triangles, m.centroid,
       m.area, m.volume, m.mesh_filename, m.neurons_idx, m.synapses_idx);
}

/**
 * \brief Payload description of a vertex of type Segment
 */
struct segment_t {
    segment_t() = default;
    segment_t(uint32_t section_id,
              uint32_t segment_id,
              uint8_t type,
              float x1,
              float y1,
              float z1,
              float r1,
              float x2,
              float y2,
              float z2,
              float r2);

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
                                             uint32_t segment_id,
                                             uint8_t type,
                                             float x1,
                                             float y1,
                                             float z1,
                                             float r1,
                                             float x2,
                                             float y2,
                                             float z2,
                                             float r2);
    pybind11::array serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a segment_t
template <class Archive>
void serialize(Archive& ar, segment_t& s) {
    ar(s.section_id, s.segment_id, s.type, s.x1, s.y1, s.z1, s.r1, s.x2, s.y2, s.z2, s.r2);
}

/**
 * \brief Payload description of an edge between an Astrocyte and a Segment
 */
struct edge_astrocyte_segment_t {
    edge_astrocyte_segment_t() = default;
    edge_astrocyte_segment_t(const float_point_t& astrocyte, const float_point_t& vasculature);
    // endfoot starting point on morphology
    float_point_t astrocyte;

    // endfoot ending point on vasculature surface
    float_point_t vasculature;

    static std::unique_ptr<edge_astrocyte_segment_t> create(const float_point_t& astrocyte,
                                                            const float_point_t& vasculature);
    pybind11::array serialize_sstream() const;
    void deserialize_sstream(pybind11::array_t<char>& data);
};

/// Cereal serialization method for \a edge_astrocyte_segment_t
template <class Archive>
void serialize(Archive& ar, edge_astrocyte_segment_t& a) {
    ar(a.astrocyte, a.vasculature);
}

/**
 *  \}
 */

}  // namespace circuit
