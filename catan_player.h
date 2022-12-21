#ifndef __CATAN_PLAYER_H__
#define __CATAN_PLAYER_H__
//header file content
#include <cstddef>
#include <unordered_set>
#include "policy.h"
#include "catan_game_board.h"


const int NUM_RESOURCES = 5;
const int NUM_DEVELOPMENT_CARDS = 1;

// forward declare
class GameState;

class Player {

    public:
        PlayerPolicy *player_policy;

        int resource_cards [NUM_RESOURCES];     //wheat, wood, wool, ore, brick
        int dev_cards [NUM_DEVELOPMENT_CARDS];  // Victory points

        int card_count;
        int victory_points;

        std::unordered_set <HexPath, HashPath> roads;
        std::unordered_set <HexPath, HashPath> road_sites;
        std::unordered_set <HexIntersection, HashIntersection> settlements;
        std::unordered_set <HexIntersection, HashIntersection> settlement_sites;
        std::unordered_set <HexIntersection, HashIntersection> cities;


        // constructor
        Player(PlayerPolicy *policy);
        Player(Player *player);
        Player() {};

        // Given the current game state, returns a move the player will make following its policy
        GameState *get_player_move(GameState *game_state);

        // overloads
        bool operator==(const Player& player) const;

};

class HashPlayer {
    public:
        size_t operator()(const Player& player) const;
};

#endif