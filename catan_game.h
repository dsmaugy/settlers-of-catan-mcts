#ifndef __CATAN_GAME_H__
#define __CATAN_GAME_H__
//header file content
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "catan_player.h"
#include "hex.h"

class GameState{

    public:
        Player player_one;
        Player player_two;
        Player current_turn;
        Hex robber_position;

        static std::unordered_set<Hex> tiles;
        static std::unordered_map<Hex, int> tile_rewards;
    
        bool is_game_over();
        Player game_winner();
        std::vector<GameState> get_all_moves();
};

class Game {

    public:
        GameState game_state;
};

#endif