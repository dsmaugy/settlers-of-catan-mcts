#ifndef __CUSTOM_HASH_GUARD
#define __CUSTOM_HASH_GUARD
//header file content

#include <stddef.h>
#include "hex.h"

class HashHex {
    public:
        size_t operator()(const Hex& hex) const;
};

class HashRoads {
    public:
        size_t operator()(const HexPath& road) const;
};

class HashBuildings {
    public:
        size_t operator()(const HexIntersection& building) const;
};
#endif 