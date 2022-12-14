#include "catan_game_board.h"
#include <stdexcept>
#include <array>
#include <algorithm>
// #include <iostream>

enum Axis {
    Q = 0, R = 1, S = 2
};

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

    if (h1.q == h2.q) axis = Axis::Q;
    if (h1.r == h2.r) axis = Axis::R;
    if ((0 - h1.q - h1.r) == (0 - h2.q - h2.r)) axis = Axis::S;


}

bool HexPath::operator==(const HexPath& hp) const {
    return (hp.hex_one == hex_one && hp.hex_two == hex_two);
}

bool HexPath::operator<(const HexPath& hp) const {
    return axis < hp.axis;
}

HexIntersection::HexIntersection(HexPath h1, HexPath h2) {
    if (h1.axis == Axis:: Q || h2.axis == Axis::S) {
        path_one = h1;
        path_two = h2;
    }
    if (h1.axis == Axis::S || h2.axis == Axis::Q) {
        path_one = h2;
        path_two = h1;
    }

    num_paths = 2;
}

HexIntersection::HexIntersection(HexPath h1, HexPath h2, HexPath h3) {
    std::array<HexPath, 3> paths = {h1, h2, h3};
    std::sort(paths.begin(), paths.end());

    // std::cout << "r1: " << paths[0].axis << ", r2: " << paths[1].axis << ", r3: " << paths[2].axis << std::endl;
    path_one = paths[0];
    path_two = paths[1];
    path_three = paths[2];
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