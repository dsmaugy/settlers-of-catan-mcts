#ifndef __HEX_GUARD
#define __HEX_GUARD
//header file content

class Hex {
    // represents a hexagonal tile on the catan board
    public:
        int q, r, s;

        Hex(int q, int r, int s);
        bool operator==(const Hex& hex) const;
};

#endif