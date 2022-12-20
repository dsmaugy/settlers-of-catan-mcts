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

        // Initialize the tiles unordered set
        // static unordered_set<Hex, HashHex> init_set();
        
        // Initialize the tile_rewards unordered map
        // static unordered_set<Hex, int, HashHex> init_map();

        // Return true if game is over, false otherwise
        bool is_game_over();

        // If terminal state, return the Player object of the winner
        Player game_winner();
        
        // allocate all new GameState objects on heap, users need to free it
        std::vector<GameState*> get_all_moves(); 

        // constructor 
        GameState(Player p1, Player p2, Hex robber_pos, int turn);
        GameState() {};

        // overloads
        bool operator==(const GameState& state) const;

};

// GameState::std::unordered_set<Hex, HashHex> tiles = {};
// GameState::std::unordered_map<Hex, int, HashHex> tile_rewards = {};

class Game {

    public:
        GameState game_state;

        /* 
            Given a state, roll the dice and update resource values for each player
            This should be called BEFORE calling a player's get_player_move() function so the player know's what state their in after getting the resources
            This will also be called by the MCTS policy to simulate future game states 
        */
        static void update_state_with_dice_roll(GameState *state);

        // constructor
        Game(Player p1, Player p2);
        Game() {};
};

class HashGameState {
    public:
        size_t operator()(const GameState& state) const;
};

#endif