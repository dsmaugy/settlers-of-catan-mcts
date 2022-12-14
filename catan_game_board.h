#ifndef __CATAN_GAME_BOARD_H_
#define __CATAN_GAME_BOARD_H_
//header file content

class Hex {
    // represents a hexagonal tile on the catan board
    public:
        int q, r;

        Hex() {};
        Hex(int q, int r);
        bool operator==(const Hex& hex) const;
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

class HexIntersection {
    public:
        HexPath path_one, path_two, path_three;
        int num_paths;

        HexIntersection() {};
        HexIntersection(HexPath h1, HexPath h2);
        HexIntersection(HexPath h1, HexPath h2, HexPath h3);

        bool operator==(const HexIntersection& hex_int) const;
};

#endif