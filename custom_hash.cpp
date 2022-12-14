#include "custom_hash.h"

size_t HashHex::operator()(const Hex& hex) const {
    return ((std::hash<int>()(hex.q)
            ^ (std::hash<int>()(hex.r) << 1)
            ^ (std::hash<int>()(hex.s) << 1)));
};

size_t HashPath::operator()(const HexPath& road) const {
    return ((HashHex()(road.hex_one)
            ^ (HashHex()(road.hex_two)) << 1));
};

size_t HashIntersection::operator()(const HexIntersection& building) const {
    size_t hash_value;
    hash_value = (HashPath()(building.path_one)
                    ^ (HashPath()(building.path_two)) << 1);
    
    if (building.num_paths == 2) {
        return hash_value;
    } else {
        return hash_value^HashPath()(building.path_three) << 1;
    }
};

