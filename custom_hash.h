#ifndef __CUSTOM_HASH_GUARD
#define __CUSTOM_HASH_GUARD
//header file content

#include <stddef.h>
#include "catan_game_board.h"

class HashHex {
    public:
        size_t operator()(const Hex& hex) const;
};

class HashPath {
    public:
        size_t operator()(const HexPath& road) const;
};

class HashIntersection {
    public:
        size_t operator()(const HexIntersection& building) const;
};
#endif 