#ifndef __CATAN_GAME_H__
#define __CATAN_GAME_H__
//header file content
#include <unordered_set>
#include <unordered_map>
#include "catan_player.h"

class GameState{

public:
    Player player_one;
    Player player_two;
    Player current_turn;
    Hex robber_position;

    static unordered_set <Hex> tiles;
    static unordered_map <Hex, int> tile_rewards;
 
    bool is_game_over();
    Player game_winner();
    Vector<GameState> get_all_moves();
};

class Game {

public:
    GameState game_state;
};

#endif