#include "custom_hash.h"

size_t HashHex::operator()(const Hex& hex) const {
    return ((std::hash<int>()(hex.q)
            ^ (std::hash<int>()(hex.r) << 1)
            ^ (std::hash<int>()(hex.s) << 1)));
};

size_t HashRoads::operator()(const std::pair<Hex, Hex>& road) const {
    return ((HashHex()(road.first)
            ^ (HashHex()(road.second)) << 1));
};

size_t HashBuildings::operator()(const std::vector<std::pair<Hex, Hex>>& building) const {
    size_t hash_value;
    for (std::pair<Hex, Hex> road: building) {
        hash_value += HashRoads()(road);
        hash_value <<= 1;
    }

    return hash_value;
};

