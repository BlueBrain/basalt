#ifndef BASALT_NETWORK_HXX
#define BASALT_NETWORK_HXX

#include <sstream>

#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

template <typename Payload>
status_t nodes_t::insert(node_t type, node_id_t id, const Payload& data,
                         node_uid_t& res, bool commit) {
    std::ostringstream oss;
    data.serialize(oss);
    // \fixme TCL get buffer beginning and length from ostringstream
    // to avoid extra std::string copy.
    const std::string raw(oss.str());
    return insert(type, id, raw.c_str(), raw.size(), res, commit);
}

/**
 * \brief Retrieve a node from the graph
 * \tparam T node payload type
 * \param uid node unique identifier
 * \param payload object updated if node is present
 * \return information whether operation succeeded or not
 */
template <typename T>
status_t nodes_t::get(const node_uid_t& uid, T& payload) const {
    std::string data;
    auto const& status = get(uid, &data);
    if (status) {
        std::istringstream istr;
        istr.rdbuf()->pubsetbuf(const_cast<char*>(data.c_str()), data.size());
        payload.deserialize(istr);
    }
    return status;
}

} // namespace basalt

#endif // !BASALT_NETWORK_HXX
