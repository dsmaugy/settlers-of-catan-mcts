#ifndef __CATAN_PLAYER_H__
#define __CATAN_PLAYER_H__
//header file content
#include <unordered_set>
#include "hex.h"

class GameState;

const int NUM_RESOURCES = 5;
const int NUM_DEVELOPMENT_CARDS = 2;


class Player {

    public:
        PlayerPolicy player_policy;

        static std::unordered_set <HexPath> roads;
        static std::unordered_set <HexIntersection> settlements;
        static std::unordered_set <HexIntersection> cities;

        int resource_cards [NUM_RESOURCES];
        int dev_cards [NUM_DEVELOPMENT_CARDS];

        GameState get_player_move(GameState game_state);


};

#endif