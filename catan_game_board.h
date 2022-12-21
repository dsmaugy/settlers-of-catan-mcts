#ifndef __CATAN_GAME_BOARD_H_
#define __CATAN_GAME_BOARD_H_
//header file content

#include <cstddef>
#include <unordered_set>

class HashHex;

class Hex {
    // represents a hexagonal tile on the catan board
    public:
        int q, r, land_type; // [0-4], -1 = OOB/Desert

        Hex() {};
        Hex(int q, int r, int land_type);
        bool operator==(const Hex& hex) const;
        bool operator!=(const Hex& hex) const;
};

class HexPath {
    public:
        Hex hex_one, hex_two;
        int axis;
        

        HexPath() {};
        HexPath(Hex h1, Hex h2);
        bool operator==(const HexPath& hp) const;
        bool operator< (const HexPath& hp) const;
};

class HashHex {
    public:
        size_t operator()(const Hex& hex) const;
};

class HexIntersection {
    public:
        HexPath path_one, path_two, path_three;
        int num_paths;
        std::unordered_set<Hex, HashHex> adjacent;

        HexIntersection() {};
        HexIntersection(HexPath h1, HexPath h2);
        HexIntersection(HexPath h1, HexPath h2, HexPath h3);

        bool operator==(const HexIntersection& hex_int) const;
};

// CUSTOM HASHING DEFINITIONS


class HashPath {
    public:
        size_t operator()(const HexPath& road) const;
};

class HashIntersection {
    public:
        size_t operator()(const HexIntersection& building) const;
};

#endif