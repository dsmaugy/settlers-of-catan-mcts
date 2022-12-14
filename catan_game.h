#ifndef __CATAN_GAME_H__
#define __CATAN_GAME_H__
//header file content
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "catan_player.h"
#include "catan_game_board.h"
#include "custom_hash.h"

class GameState{

    public:
        Player player_one;
        Player player_two;

        Hex robber_position;

        int current_turn;

        static std::unordered_set<Hex, HashHex> tiles;
        static std::unordered_map<Hex, int, HashHex> tile_rewards;
    
        bool is_game_over();
        Player game_winner();
        std::vector<GameState> get_all_moves();
};

class Game {

    public:
        GameState game_state;
};

#endif