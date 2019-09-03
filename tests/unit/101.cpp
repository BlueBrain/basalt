#include <cstdlib>
#include <stdexcept>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <basalt/basalt.hpp>

using basalt::DirectedGraph;
using basalt::edge_uid_t;
using basalt::Graph;
using basalt::make_id;
using basalt::UndirectedGraph;
using basalt::vertex_id_t;
using basalt::vertex_t;
using basalt::vertex_uid_t;
using basalt::vertex_uids_t;

/**
 * \brief Helper function adding a vertex to a graph and checking operation
 * succeeded
 * \param g graph
 * \param type vertex type
 * \param id vertex identifier
 * \param payload vertex content
 * \return vertex unique identifier
 */
template <typename Payload, basalt::EdgeOrientation Ordered>
inline vertex_uid_t checked_insert(Graph<Ordered>& g,
                                   vertex_t type,
                                   vertex_id_t id,
                                   const Payload& payload) {
    const auto uid = make_id(type, id);
    const auto result = g.vertices().insert(uid, payload);
    REQUIRE(result);
    return uid;
}

/**
 * \brief Helper function adding a vertex to a graph and checking operation
 * succeeded
 * \param g graph
 * \param type vertex type
 * \param id vertex identifier
 * \return vertex unique identifier
 */
inline vertex_uid_t checked_insert(UndirectedGraph& g, vertex_t type, vertex_id_t id) {
    auto uid = make_id(type, id);
    const auto result = g.vertices().insert(uid);
    REQUIRE(result);
    return uid;
}

static void check_is_ok(const basalt::Status& status) {
    REQUIRE(status);
}

namespace bbp {
namespace in_silico {

/** \name vertex payloads
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
        return ostr << version << pre_gid << post_gid << nrn_idx << is_excitatory << pre_x << pre_y
                    << pre_y << pre_z << post_x << post_y << post_z;
    }

    void deserialize(std::istream& istr) {
        istr >> version >> pre_gid >> post_gid >> nrn_idx >> is_excitatory >> pre_x >> pre_y >>
            pre_y >> pre_z >> post_x >> post_y >> post_z;
    }
};

/** \} */

}  // namespace in_silico
}  // namespace bbp

/// \brief different types of vertex
enum vertex_type { synapse, segment, astrocyte };

static std::string new_db_path() {
    char db_path[] = "/tmp/basalt-ut-XXXXXX";
    if (mkdtemp(static_cast<char*>(db_path)) == nullptr) {
        throw std::runtime_error(strerror(errno));
    }
    return static_cast<char*>(db_path);
}

TEST_CASE("one-vertex-db", "[GraphKV]") {
    const auto path = new_db_path();
    const auto vertex = make_id(42, 3);
    {
        UndirectedGraph g(path);
        REQUIRE(std::distance(g.vertices().begin(), g.vertices().end()) == 0);
        {
            std::string data;
            REQUIRE(g.vertices().get(vertex, &data).code == basalt::Status::missing_vertex_code);
            REQUIRE(data.empty());
        }

        g.vertices().insert(vertex).raise_on_error();
    }
    {
        UndirectedGraph g(path);
        REQUIRE(std::distance(g.vertices().begin(), g.vertices().end()) == 1);
        {
            std::string data;
            g.vertices().get(vertex, &data).raise_on_error();
            REQUIRE(data.empty());
        }
        {
            bool exists = false;
            g.vertices().has(vertex, exists).raise_on_error();
            REQUIRE(exists);
        }
    }
    {
        // try to remove an inexistant vertex
        UndirectedGraph g(path);
        REQUIRE(std::distance(g.vertices().begin(), g.vertices().end()) == 1);
        g.vertices().erase(vertex).raise_on_error();
        REQUIRE(std::distance(g.vertices().begin(), g.vertices().end()) == 0);
    }
}

TEST_CASE("ordered graph", "[GraphKV]") {
    using bbp::in_silico::synapse_t;

    DirectedGraph g(new_db_path());

    // add synapses with id 0 and 1
    const auto s0 = checked_insert(g,
                                   vertex_type::synapse,
                                   0,
                                   synapse_t{0, 42, 42, 42, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    const auto s1 = checked_insert(g,
                                   vertex_type::synapse,
                                   1,
                                   synapse_t{0, 43, 43, 43, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    // connect the 2 synapses together
    check_is_ok(g.edges().insert(s0, s1));

    bool result = false;
    REQUIRE(g.edges().has(s0, s1, result));
    REQUIRE(result);
    REQUIRE(g.edges().has(s1, s0, result));
    REQUIRE(!result);
}

TEST_CASE("create simple GraphKV and check entities", "[GraphKV]") {
    using basalt::UndirectedGraph;
    using bbp::in_silico::synapse_t;

    UndirectedGraph g(new_db_path());

    // add synapses with id 0 and 1
    const auto s0 = checked_insert(g,
                                   vertex_type::synapse,
                                   0,
                                   synapse_t{0, 42, 42, 42, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    const auto s1 = checked_insert(g,
                                   vertex_type::synapse,
                                   1,
                                   synapse_t{0, 43, 43, 43, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    // add astrocytes
    const auto a0 = checked_insert(g, vertex_type::astrocyte, 0);
    const auto a1 = checked_insert(g, vertex_type::astrocyte, 1);

    check_is_ok(g.commit());

    {
        std::set<basalt::vertex_uid_t> all_vertices;
        for (auto const& vertex: g.vertices()) {
            all_vertices.insert(vertex);
        }
        REQUIRE(all_vertices.size() == 4);
    }

    // connect the 2 synapses together
    check_is_ok(g.edges().insert(s0, s1));

    // connect synapse 0 to the 2 astrocytes
    check_is_ok(g.edges().insert(s0, {a0, a1}));

    // connect 2 astrocyte segments
    check_is_ok(g.edges().insert(a0, a1));

    vertex_uids_t vertices;
    // get all Vertices connected to synapse 0
    check_is_ok(g.edges().get(s0, vertices));
    REQUIRE(vertices.size() == 3);
    for (auto const& vertex: vertices) {
        std::cout << s0 << " ↔ " << vertex << std::endl;
        // (S:0) ↔ (S:1)
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    vertices.clear();
    // only get astrocytes connected to synapse 0
    check_is_ok(g.edges().get(s0, vertex_type::astrocyte, vertices));
    REQUIRE(vertices.size() == 2);
    for (auto const& astrocyte: vertices) {
        std::cout << s0 << " ↔ " << astrocyte << std::endl;
        // (S:0) ↔ (A:0)
        // (S:0) ↔ (A:1)
    }

    {
        // iterate over all vertices
        std::set<vertex_uid_t> vertices_set;
        int count = 0;
        for (const auto& vertex: g.vertices()) {
            ++count;
            vertices_set.insert(vertex);
        }
        REQUIRE(count == 4);
        REQUIRE(vertices_set.size() == 4);
    }

    {
        // test count of vertices
        auto count = 0lu;
        g.vertices().count(count).raise_on_error();
        REQUIRE(count == 4);
    }
    {
        // iterator over second half of vertices
        auto count = 0lu;
        const auto vertices_end = g.vertices().end();
        for (auto vertex = g.vertices().begin(2); vertex != vertices_end; ++vertex) {
            ++count;
        }
        REQUIRE(count == 2);
    }

    {
        // iterate over all edges
        std::set<edge_uid_t> edges_set;
        const std::set<edge_uid_t> expected{
            std::make_pair(make_id(vertex_type::synapse, 0), make_id(vertex_type::synapse, 1)),
            std::make_pair(make_id(vertex_type::synapse, 1), make_id(vertex_type::synapse, 0)),

            std::make_pair(make_id(vertex_type::synapse, 0), make_id(vertex_type::astrocyte, 0)),
            std::make_pair(make_id(vertex_type::astrocyte, 0), make_id(vertex_type::synapse, 0)),

            std::make_pair(make_id(vertex_type::synapse, 0), make_id(vertex_type::astrocyte, 1)),
            std::make_pair(make_id(vertex_type::astrocyte, 1), make_id(vertex_type::synapse, 0)),

            std::make_pair(make_id(vertex_type::astrocyte, 0), make_id(vertex_type::astrocyte, 1)),
            std::make_pair(make_id(vertex_type::astrocyte, 1), make_id(vertex_type::astrocyte, 0))};
        int count = 0;
        for (const auto& edge: g.edges()) {
            ++count;
            edges_set.insert(edge);
        }
        // every edge is count twice
        REQUIRE(count == 8);
        REQUIRE(edges_set.size() == 8);
        REQUIRE(edges_set == expected);
    }
}
