#include <iostream>
#include <unordered_map>

#include "catan_game.h"
#include "custom_hash.h"

#include <chrono>

#define TIME_DIFF_MILLISECONDS(start, end) (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())

int main(int argc, char** argv) {
    std::cout << "hello world" << "df\n";

    // args: p1_model p2_model time_limit num_games



    Hex hex_one = Hex(0, 0);
    Hex hex_two = Hex(1, -1);
    Hex hex_three = Hex(1, 0);
    Hex hex_four = Hex(0, -1);

    HexPath road_one = HexPath(hex_one, hex_two);
    HexPath road_two = HexPath(hex_one, hex_three);
    HexPath road_three = HexPath(hex_two, hex_three);
    HexPath road_four = HexPath(hex_one, hex_four);
    HexPath road_five = HexPath(hex_two, hex_four);

    HexPath road_one_equivalent = HexPath(hex_two, hex_one);


    // std::unordered_map<Hex, int, HashHex> hex_map;
    // hex_map[hex_one] = -1;

    std::unordered_map<HexPath, int, HashPath> road_map;
    road_map[road_one] = 21;
    road_map[road_two] = 22;
    road_map[road_three] = 23;
    road_map[road_four] = 24;
    road_map[road_five] = 25;

    // std::unordered_map<HexIntersection, int, HashIntersection> building_map;
    //     HexIntersection building_two = {road_one, road_two};

    HexIntersection building_one = {road_one, road_two, road_three};
    HexIntersection building_two = {road_one, road_four, road_five};

    std::cout << " r1: " << road_map[building_one.path_one] << ", r2: " << road_map[building_one.path_two] << ", r3: " << road_map[building_one.path_three] <<std::endl;
    std::cout << " r1: " << road_map[building_two.path_one] << ", r2: " << road_map[building_two.path_two] << ", r3: " << road_map[building_two.path_three] <<std::endl;

    // building_map[building_one] = 18;
    // building_map[building_two] = 17;


    // std::cout << "Hex One val: " << hex_map[hex_one] << std::endl;
    // std::cout << "Road One: " << road_map[road_one] << " Road Two: " << road_map[road_two] << " Road Three: " << road_map[road_three] << std::endl;
    // std::cout << "Building One: " << building_map[building_one] << " Building Two: " << building_map[building_two] << std::endl;
    // std::cout << "Hex Path equivalence associativity: " << (road_one_equivalent == road_one) << std::endl;
    return 0;
}