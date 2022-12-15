#ifndef __CATAN_GAME_H__
#define __CATAN_GAME_H__
//header file content
#include <cstddef>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "catan_game_board.h"
#include "catan_player.h"


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
        
        // allocate all new GameState objects on heap, users need to free it
        std::vector<GameState*> get_all_moves(); 

        // overloads
        bool operator==(const GameState& state) const;

};

class Game {

    public:
        GameState game_state;
        static std::unordered_set<Hex, HashHex> tiles;
        static std::unordered_map<Hex, int, HashHex> tile_rewards;
        Game() {};
        Game(Player p1, Player p2);
};

class HashGameState {
    public:
        size_t operator()(const GameState& state) const;
};

#endif