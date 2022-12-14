#ifndef __HEX_GUARD
#define __HEX_GUARD
//header file content

#include <vector>

class Hex {
    // represents a hexagonal tile on the catan board
    public:
        int q, r, s;

        Hex() {};
        Hex(int q, int r, int s);
        bool operator==(const Hex& hex) const;
};

class HexPath {
    public:
        Hex hex_one, hex_two;

        HexPath() {};
        HexPath(Hex h1, Hex h2);
        bool operator==(const HexPath& hp) const;
};

class HexIntersection {
    public:
        HexPath path_one, path_two, path_three;
        int num_paths;

        HexIntersection() {};
        HexIntersection(HexPath h1, HexPath h2);
        HexIntersection(HexPath h1, HexPath h2, HexPath h3);

        bool operator==(const HexIntersection& hex_int) const;
};


// typedef std::pair<Hex, Hex> HexPath;
// typedef std::vector<HexPath> HexIntersection;

#endif