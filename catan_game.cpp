#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"
#include <unordered_set>
#include <algorithm>
#include <random>       // std::default_random_engine

int VALUES[18] = {5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11};
int seed = 12345;

Game::Game(Player p1, Player p2) {
    // build all of the hexes, put into tiles

    std::shuffle(std::begin(VALUES), std::end(VALUES), std::default_random_engine(seed));


    int r1 = 0, r2 = 3;

    for (int q = -3; q <= 3; q++) {
        for (int r = r1; r <= r2; r++) {
            Hex h = Hex(q,r);
            tiles.insert(h);

        }
    }
}

// Keep a list of possible road locations, update as you build roads
// Same for settlement sites

// Finding adjacent roads: depending on orientation of built road, you can figure out the other two
// that are connected to it.

// Possible moves
// Build a road
// Build a settlemnt
// Upgrade a settlment -> City
// Build a dev card (but not use it right away)
// Use prev held dev cards


// Turn possibility
// 
// CAN build up to 1 item
// CAN play a dev card
// or you can do nothing for either of those

// Next move func

//  roll die, collect resources for all players
//  for current player, feed current GameState to get_player_move to get that player's move
//  Update the GameState to the one
// returned from the player's policy
// (need to free the old GameState as the one returned from the player's policy will have been allocated on the heap by get_all_moves())







// CUSTOM HASHING

size_t HashPlayer::operator()(const Player& player) const {
    size_t cities_hash, settlements_hash, roads_hash, resource_hash, dev_cards_hash;
    cities_hash = settlements_hash = roads_hash = resource_hash = dev_cards_hash = 0;

    for (const auto& cities : player.cities)
        cities_hash ^= HashIntersection()(cities);
    

    for (const auto& settlements : player.settlements) 
        settlements_hash ^= HashIntersection()(settlements);
    
    for (const auto& road : player.roads)
        roads_hash ^= HashPath()(road);

    for (int i: player.resource_cards) 
        resource_hash ^= (i << 1);
    

    for (int i : player.dev_cards) 
        dev_cards_hash ^= (i << 1);
    
    return (std::hash<size_t>()(cities_hash)
            ^ (std::hash<size_t>()(settlements_hash) << 1)
            ^ (std::hash<size_t>()(roads_hash) << 1)
            ^ (std::hash<size_t>()(resource_hash) << 1)
            ^ (std::hash<size_t>()(dev_cards_hash) << 1));

};


size_t HashGameState::operator()(const GameState& state) const {
    return (std::hash<int>()(state.current_turn)
            ^ (HashPlayer()(state.player_one) << 1)
            ^ (HashPlayer()(state.player_two) << 1)
            ^ (HashHex()(state.robber_position)));
};
