#include <cstdlib>
#include <stdexcept>

#include "stdlib.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <basalt/basalt.hpp>

using basalt::network_t;
using basalt::node_id_t;
using basalt::node_t;
using basalt::node_uid_t;
using basalt::node_uids_t;

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
inline node_uid_t checked_insert(network_t& g, node_t type, node_id_t id,
                                 const Payload& payload) {
    node_uid_t nuid;
    const auto result = g.nodes().insert(type, id, payload, nuid);
    REQUIRE(result);
    return nuid;
}

/**
 * \brief Helper function adding a node to a graph and checking operation
 * succeeded
 * \param g graph
 * \param type node type
 * \param id node identifier
 * \return node unique identifier
 */
inline node_uid_t checked_insert(network_t& g, node_t type, node_id_t id) {
    node_uid_t nuid;
    const auto result = g.nodes().insert(type, id, nuid);
    REQUIRE(result);
    return nuid;
}

static void check_is_ok(const basalt::status_t& status) { REQUIRE(status); }

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

    std::ostream& serialize(std::ostream& ostr) const {
        return ostr << version << pre_gid << post_gid << nrn_idx
                    << is_excitatory << pre_x << pre_y << pre_y << pre_z
                    << post_x << post_y << post_z;
    }

    void deserialize(std::istream& istr) {
        istr >> version >> pre_gid >> post_gid >> nrn_idx >> is_excitatory >>
            pre_x >> pre_y >> pre_y >> pre_z >> post_x >> post_y >> post_z;
    }
};

/** \} */

} // namespace in_silico
} // namespace bbp

/// \brief different types of node
enum node_type { synapse, segment, astrocyte };

static std::string new_db_path() {
    char db_path[] = "/tmp/basalt-ut-XXXXXX";
    if (mkdtemp(db_path) == nullptr) {
        throw std::runtime_error(strerror(errno));
    };
    return db_path;
}

TEST_CASE("one-node-db", "[graph]") {
    const auto path = new_db_path();
    node_uid_t node;
    {
        network_t g(path);
        REQUIRE(std::distance(g.nodes().begin(), g.nodes().end()) == 0);
        {
            std::string data;
            REQUIRE(g.nodes().get(node, &data).code ==
                    basalt::status_t::missing_node_code);
            REQUIRE(data.empty());
        }

        g.nodes().insert(42, 3, node).raise_on_error();
    }
    REQUIRE(node.first == 42);
    REQUIRE(node.second == 3);
    {
        network_t g(path);
        REQUIRE(std::distance(g.nodes().begin(), g.nodes().end()) == 1);
        {
            std::string data;
            g.nodes().get(node, &data).raise_on_error();
            REQUIRE(data.empty());
        }
        {
            bool exists = false;
            g.nodes().has(node, exists).raise_on_error();
            REQUIRE(exists);
        }
    }
    {
        // try to remove an inexistant node
        network_t g(path);
        REQUIRE(std::distance(g.nodes().begin(), g.nodes().end()) == 1);
        g.nodes().erase(node).raise_on_error();
        REQUIRE(std::distance(g.nodes().begin(), g.nodes().end()) == 0);
    }
}

TEST_CASE("create simple graph and check entities", "[graph]") {
    using basalt::network_t;
    using namespace bbp::in_silico;

    network_t g(new_db_path());

    // add synapses with id 0 and 1
    const auto s0 = checked_insert(
        g, node_type::synapse, 0,
        synapse_t{0, 42, 42, 42, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    const auto s1 = checked_insert(
        g, node_type::synapse, 1,
        synapse_t{0, 43, 43, 43, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    // add astrocytes
    const auto a0 = checked_insert(g, node_type::astrocyte, 0);
    const auto a1 = checked_insert(g, node_type::astrocyte, 1);

    check_is_ok(g.commit());

    {
        std::set<basalt::node_uid_t> all_nodes;
        for (auto const& node : g.nodes()) {
            all_nodes.insert(node);
        }
        REQUIRE(all_nodes.size() == 4);
    }

    // connect the 2 synapses together
    check_is_ok(g.connections().insert(s0, s1));

    // connect synapse 0 to the 2 astrocytes
    check_is_ok(g.connections().insert(s0, {a0, a1}));

    // connect 2 astrocyte segments
    check_is_ok(g.connections().insert(a0, a1));

    node_uids_t nodes;
    // get all nodes_t connected to synapse 0
    check_is_ok(g.connections().get(s0, nodes));
    REQUIRE(nodes.size() == 3);
    for (auto const& node : nodes) {
        std::cout << s0 << " ↔ " << node << std::endl;
        // (S:0) ↔ (S:1)
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    nodes.clear();
    // only get astrocytes connected to synapse 0
    check_is_ok(g.connections().get(s0, node_type::astrocyte, nodes));
    REQUIRE(nodes.size() == 2);
    for (auto const& astrocyte : nodes) {
        std::cout << s0 << " ↔ " << astrocyte << std::endl;
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    {
        // iterate over all nodes
        std::set<node_uid_t> nodes_set;
        int count = 0;
        for (const auto& node : g.nodes()) {
            std::cout << node << std::endl;
            ++count;
            nodes_set.insert(node);
        }
        REQUIRE(count == 4);
        REQUIRE(nodes_set.size() == 4);
    }

    // iterator over second of nodes
    auto count = 0lu;
    REQUIRE_THROWS_AS(g.nodes().count(count).raise_on_error(),
                      std::runtime_error);
    const auto nodes_end = g.nodes().end();
    for (auto node = g.nodes().begin(count / 2); node != nodes_end; ++node) {
        std::cout << *node << std::endl;
    }
}
