#ifndef __CUSTOM_HASH_GUARD
#define __CUSTOM_HASH_GUARD
//header file content

#include <stddef.h>
#include <vector>
#include "hex.h"

class HashHex {
    public:
        size_t operator()(const Hex& hex) const;
};

class HashRoads {
    public:
        size_t operator()(const std::pair<Hex, Hex>& road) const;
};

class HashBuildings {
    public:
        size_t operator()(const std::vector<std::pair<Hex, Hex>>& building) const;
};
#endif 