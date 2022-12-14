#include "catan_game_board.h"
#include <stdexcept>

Hex::Hex(int q_coord, int r_coord) {
    if (abs(q_coord) > 2 || abs(r_coord) > 2)
        throw std::invalid_argument("Hex coords out of Catan range.");

    q = q_coord;
    r = r_coord;
}

bool Hex::operator==(const Hex& hex) const {
    return (q == hex.q && r == hex.r);
}

HexPath::HexPath(Hex h1, Hex h2) {
    if ((h1.r < h2.r) || (h1.r == h2.r && h1.q < h2.q)) {
        hex_one = h1;
        hex_two = h2;
    } else {
        hex_one = h2;
        hex_two = h1;
    }
}

bool HexPath::operator==(const HexPath& hp) const {
    return (hp.hex_one == hex_one && hp.hex_two == hex_two);
}

HexIntersection::HexIntersection(HexPath h1, HexPath h2) {
    path_one = h1;
    path_two = h2;
    num_paths = 2;
}

HexIntersection::HexIntersection(HexPath h1, HexPath h2, HexPath h3) {
    path_one = h1;
    path_two = h2;
    path_three = h3;
    num_paths = 3;
}

bool HexIntersection::operator==(const HexIntersection& hex_int) const {
    bool first_two_paths_equal = hex_int.path_one == path_one && hex_int.path_two == path_two;
    if (num_paths == 2) {
        return first_two_paths_equal;
    } else {
        return first_two_paths_equal && hex_int.path_three == path_three; 
    }
    // TODO: this might be dangerous?? 
}