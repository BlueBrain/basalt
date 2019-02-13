//
// Created by magkanar on 2/7/19.
//

#include <benchmark/benchmark.h>
#include <basalt/basalt.hpp>
#include <string>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <set>

using basalt::network_t;
using basalt::node_id_t;
using basalt::node_t;
using basalt::node_uid_t;
using basalt::node_uids_t;

namespace bbp {
    namespace in_silico {

/** \name node payloads
 ** \{
 */

        struct synapse_t {
            unsigned char version;
            uint32_t pre_gid;
            uint32_t post_gid;
            uint32_t nrn_idx;
            bool is_excitatory;
            float pre_x;
            float pre_y;
            float pre_z;
            float post_x;
            float post_y;
            float post_z;

            std::ostream &serialize(std::ostream &ostr) const {
                return ostr << version << pre_gid << post_gid << nrn_idx << is_excitatory << pre_x << pre_y
                            << pre_y << pre_z << post_x << post_y << post_z;
            }

            void deserialize(std::istream &istr) {
                istr >> version >> pre_gid >> post_gid >> nrn_idx >> is_excitatory >> pre_x >> pre_y >>
                     pre_y >> pre_z >> post_x >> post_y >> post_z;
            }
        };

        struct neuron_t {
            uint32_t gid;

            std::vector<uint32_t> astro_idx;
            std::vector<uint32_t> syn_idx;

        };

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

            std::vector<uint32_t> synapses_idx;
            std::vector<uint32_t> neurons_idx;
        };

        struct microdomain_t
        {
            uint32_t microdomain_id;
            uint32_t astrocyte_id;

            // potentially different than astrocyte actual neighbors
            // microdomains form a tesselation
            std::vector<uint32_t> neighbors;

            // mesh data
            std::vector<std::vector<float>> vertex_coordinates;
            std::vector<std::vector<uint32_t>> triangles;

            // geometric centroid, not the same as the morphology soma center
            float centroid_x;
            float centroid_y;
            float centroid_z;

            double area;
            double volume;

            // unsure if this is really essential. Meshes are created from vertex_coordinates and triangles
            std::string mesh_filename;

            // maybe it's better to access neurons from synapses indirectly?
            std::vector<uint32_t> neurons_idx;
            std::vector<uint32_t> synapses_idx;

        };

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
            // FIXME
        };

        struct edge_astrocyte_segment_t {

            // endfoot starting point on morphology
            float astrocyte_x;
            float astrocyte_y;
            float astrocyte_z;

            // endfoot ending point on vasculature surface
            float vasculature_x;
            float vasculature_y;
            float vasculature_z;

        };

        struct edge_astrocyte_synapse_t {

            uint32_t morphology_neurite_id;

        };

        struct edge_synapse_neuron_t {
            // FIXME
        };

    }
}

/// \brief different types of node
enum node_type {none, neuron, synapse, astrocyte, microdomain, segment };

static void Read_Astr_Neur(benchmark::State& state) {
    using basalt::network_t;
    using bbp::in_silico::synapse_t;

    // absolute path of basalt-db
    std::string db_path = "/nvme/";
    
    std::string user_path = getenv("USER");
    db_path.append(user_path);
    db_path.append("/");
    
    std::string job_id_path = getenv("SLURM_JOB_ID");
    db_path.append(job_id_path);

    db_path.append("/basalt-db_");

    // appending the number of max astrocytes read from the .h5 file
    db_path.append(std::to_string(state.range(0)));

    // network initialization
    network_t g(db_path);

    // reading all nodes and reading and saving all astrocytes in astrocytes_set
    std::set<node_uid_t> nodes_set;
    std::set<node_uid_t> astrocytes_set;
    int count = 0;
    for (const auto& node: g.nodes()) {
        ++count;
        nodes_set.insert(node);
        if(node.first==node_type::astrocyte) {
            astrocytes_set.insert(node);
        }
    }

    // measured and reported code
    //for (auto _ : state){	// for google benchmark version >=1.3.0
    while (state.KeepRunning()){
        for (const auto& astrocyte: astrocytes_set) {
            node_uids_t nodes;
            // reading all neurons that are connected with "astrocyte"
            auto res_n = g.connections().get(astrocyte, node_type::neuron, nodes);
        }
    }
}

static void Read_Astr_Syn(benchmark::State& state) {
    using basalt::network_t;
    using bbp::in_silico::synapse_t;

    // absolute path of basalt-db
    std::string db_path = "/nvme/";

    std::string user_path = getenv("USER");
    db_path.append(user_path);
    db_path.append("/");

    std::string job_id_path = getenv("SLURM_JOB_ID");
    db_path.append(job_id_path);

    db_path.append("/basalt-db_");

    // appending the number of max astrocytes read from the .h5 file
    db_path.append(std::to_string(state.range(0)));

    // network initialization
    network_t g(db_path);

    // reading all nodes and reading and saving all astrocytes in astrocytes_set
    std::set<node_uid_t> nodes_set;
    std::set<node_uid_t> astrocytes_set;
    int count = 0;
    for (const auto& node: g.nodes()) {
        ++count;
        nodes_set.insert(node);
        if(node.first==node_type::astrocyte) {
            astrocytes_set.insert(node);
        }
    }

    // measured and reported code
    //for (auto _ : state){	// for google benchmark version >=1.3.0
    while (state.KeepRunning()){
        for (const auto& astrocyte: astrocytes_set) {
            node_uids_t nodes;
            // reading all synapses that are connected with "astrocyte"
            auto res_n = g.connections().get(astrocyte, node_type::synapse, nodes);
        }
    }
}

BENCHMARK(Read_Astr_Neur)->Arg(1)->Arg(10)->Arg(100)->Arg(1000);
BENCHMARK(Read_Astr_Syn)->Arg(1)->Arg(10)->Arg(100)->Arg(1000);

BENCHMARK_MAIN();
