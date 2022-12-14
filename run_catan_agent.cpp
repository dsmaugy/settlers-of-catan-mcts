#include <iostream>
#include <unordered_map>

#include "custom_hash.h"

int main(int argc, char** argv) {
    std::cout << "hello world" << "df\n";

    Hex hex_one = Hex(1, 2, 3);
    Hex hex_two = Hex(0, 1, 2);
    Hex hex_three = Hex(3, 4, 5);
    Hex hex_four = Hex(5, 2, 4);

    HexPath road_one = HexPath(hex_one, hex_two);
    HexPath road_two = HexPath(hex_one, hex_three);
    HexPath road_three = HexPath(hex_one, hex_four);


    std::unordered_map<Hex, int, HashHex> hex_map;
    hex_map[hex_one] = -1;

    std::unordered_map<HexPath, int, HashPath> road_map;
    road_map[road_one] = 21;
    road_map[road_two] = 22;
    road_map[road_three] = 23;

    std::unordered_map<HexIntersection, int, HashIntersection> building_map;
        HexIntersection building_two = {road_one, road_two};

    HexIntersection building_one = {road_one, road_two, road_three};
    building_map[building_one] = 18;
    building_map[building_two] = 17;


    std::cout << "Hex One val: " << hex_map[hex_one] << std::endl;
    std::cout << "Road One: " << road_map[road_one] << " Road Two: " << road_map[road_two] << " Road Three: " << road_map[road_three] << std::endl;
    std::cout << "Building One: " << building_map[building_one] << " Building Two: " << building_map[building_two] << std::endl;
    return 0;
}