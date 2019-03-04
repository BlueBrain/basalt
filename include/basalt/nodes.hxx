#ifndef BASALT_NETWORK_HXX
#define BASALT_NETWORK_HXX

#include <sstream>

#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

template <typename Payload>
status_t nodes_t::insert(node_t type,
                         node_id_t id,
                         const Payload& data,
                         node_uid_t& res,
                         bool commit) {
    std::ostringstream oss;
    data.serialize(oss);
    // \fixme TCL get buffer beginning and length from ostringstream
    // to avoid extra std::string copy.
    const std::string raw(oss.str());
    return insert(type, id, raw.c_str(), raw.size(), res, commit);
}

template <typename T>
status_t nodes_t::get(const node_uid_t& node, T& payload) const {
    std::string data;
    auto const& status = get(node, &data);
    if (status) {
        std::istringstream istr;
        istr.rdbuf()->pubsetbuf(const_cast<char*>(data.c_str()), static_cast<long>(data.size()));
        payload.deserialize(istr);
    }
    return status;
}

}  // namespace basalt

#endif  // !BASALT_NETWORK_HXX
