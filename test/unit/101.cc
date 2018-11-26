#include <cstdlib>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <basalt/basalt.hpp>

using basalt::network_t;

/**
 * \brief Helper function adding a node to a graph and checking operation
 * succeeded
 * \param g graph
 * \param type node type
 * \param id node identifier
 * \param payload node content
 * \return node unique identifier
 */
template <typename Payload>
inline network_t::node_uid_t
checked_insert(network_t& g, network_t::node_t type, network_t::node_id_t id,
               const Payload& payload) {
    const auto result = g.nodes().insert(type, id, payload);
    REQUIRE(result.second);
    return result.first;
}

static void check_is_ok(const basalt::status_t& status) { REQUIRE(status); }

namespace bbp {
namespace in_silico {

/** \name node payloads
 ** \{
 */

struct synapse_t {
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
};

struct segment_t {
    uint32_t cell_id;
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
};

struct astrocyte_t {
    // [...]
};

/** \} */

} // namespace in_silico
} // namespace bbp

/// \brief different types of node
enum node_type { synapse, segment, astrocyte };

TEST_CASE("create simple graph and check entities", "[graph]") {
    using basalt::network_t;
    using namespace bbp::in_silico;

    network_t g("");

    // add synapses with id 0 and 1
    const auto s0 = checked_insert(
        g, node_type::synapse, 0,
        synapse_t{42, 42, 42, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    const auto s1 = checked_insert(
        g, node_type::synapse, 1,
        synapse_t{43, 43, 43, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    // add astrocytes
    const auto a0 = checked_insert(g, node_type::astrocyte, 0, astrocyte_t());
    const auto a1 = checked_insert(g, node_type::astrocyte, 1, astrocyte_t());

    check_is_ok(g.commit());

    // connect the 2 synapses together
    check_is_ok(g.connections().connect(s0, s1));

    // connect synapse 0 to the 2 astrocytes
    check_is_ok(g.connections().connect(s0, {a0, a1}));

    network_t::node_uids_t nodes;
    // get all nodes_t connected to synapse 0
    check_is_ok(g.connections().get(s0, nodes));
    for (auto const& node : nodes) {
        std::cout << s0 << " ↔ " << node << std::endl;
        // (S:0) ↔ (S:1)
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    nodes.clear();
    // only get astrocytes connected to synapse 0
    check_is_ok(g.connections().get(s0, node_type::astrocyte, nodes));
    for (auto const& astrocyte : nodes) {
        std::cout << s0 << " ↔ " << astrocyte << std::endl;
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    // iterate over all nodes
    for (const auto& node: g.nodes()) {
        std::cout << node << std::endl;
    }
    // iterator over second of nodes
    auto count = 0lu;
    g.nodes().count(count).raise_on_error();
    const auto nodes_end = g.nodes().end();
    for (auto node = g.nodes().begin(count/ 2); node != nodes_end; ++node) {
        std::cout << *node << std::endl;
    }
}
