#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"

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
//  for current player, call get_all_moves(),
//  feed into that player's policy, to get that
//  player's move. Update the game state to the one
// returned from the player's policy







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
