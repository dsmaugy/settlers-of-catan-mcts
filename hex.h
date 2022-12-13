#ifndef __HEX_GUARD
#define __HEX_GUARD
//header file content

class Hex {
    public:
        int q, r, s;

        bool operator==(const Hex& hex) const;
};

#endif