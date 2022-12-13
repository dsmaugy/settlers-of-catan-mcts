#include "hex.h"

Hex::Hex(int q, int r, int s) {
    q = q;
    r = r;
    s = s;
}

bool Hex::operator==(const Hex& hex) const {
    return (q == hex.q && r == hex.r && s == hex.s);
}