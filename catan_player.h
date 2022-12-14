#ifndef __CATAN_PLAYER_H__
#define __CATAN_PLAYER_H__
//header file content
#include <unordered_set>
#include "policy.h"
// #include "catan_player_policies.h"
#include "custom_hash.h"
#include "catan_game_board.h"


const int NUM_RESOURCES = 5;
const int NUM_DEVELOPMENT_CARDS = 2;

// forward declare
class GameState;

class Player {

    public:
        PlayerPolicy player_policy;

        std::unordered_set <HexPath, HashPath> roads;
        std::unordered_set <HexIntersection, HashIntersection> settlements;
        std::unordered_set <HexIntersection, HashIntersection> cities;

        int resource_cards [NUM_RESOURCES];
        int dev_cards [NUM_DEVELOPMENT_CARDS];

        GameState get_player_move(GameState game_state);
};

#endif