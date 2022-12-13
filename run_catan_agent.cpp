#include <iostream>
#include <unordered_map>

#include "custom_hash.h"

int main(int argc, char** argv) {
    std::cout << "hello world" << "df\n";

    Hex hex_one = Hex(1, 2, 3);
    Hex hex_two = Hex(0, 1, 2);
    Hex hex_three = Hex(3, 4, 5);
    Hex hex_four = Hex(5, 2, 4);

    HexPath road_one = std::make_pair(hex_one, hex_two);
    HexPath road_two = std::make_pair(hex_one, hex_three);
    HexPath road_three = std::make_pair(hex_one, hex_four);


    std::unordered_map<Hex, int, HashHex> hex_map;
    hex_map[hex_one] = 5;

    std::unordered_map<HexPath, int, HashRoads> road_map;
    road_map[road_one] = 1;
    road_map[road_two] = 2;
    road_map[road_three] = 3;

    // std::unordered_map<>


    std::cout << "Hex One val: " << hex_map[hex_one] << "\n Road One Val: " << road_map[road_one] << std::endl;
    std::cout << "Road One: " << road_map[road_one] << " Road Two: " << road_map[road_two] << " Road Three: " << road_map[road_three] << std::endl;
    return 0;
}