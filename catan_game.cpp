#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"
#include <unordered_set>
#include <algorithm>
#include <random>       // std::default_random_engine

#define IS_IN_SET(set, elem) (set.find(elem) != set.end())

int VALUES[19] = {5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11,7}; // 7 isn't a valid reward amt, but is used to randomly assign the desert
int LAND[18] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,4,4,4};
int seed = 12345;


Game::Game(Player p1, Player p2) {
    // build all of the hexes, put into tiles

    std::shuffle(std::begin(VALUES), std::end(VALUES), std::default_random_engine(seed));
    std::shuffle(std::begin(LAND), std::end(LAND), std::default_random_engine(seed));

    // int r1 = 0, r2 = 3, i = 0;

    // for (int q = -3; q <= 3; q++) {
    //     for (int r = r1; r <= r2; r++) {
    //         Hex h = Hex(q,r); // change constructor to assign type
    //         tiles.insert(h);
    //         tile_rewards[h] = VALUES[i];

    //         i++;
    //     }
    //     if (q < 0)
    //         r1--;
    //     else
    //         r2--;
    // }
}


void Game::update_state_with_dice_roll(GameState *state) {
    // TODO
}

// GAME STATE DEFINITIONS

GameState::GameState(Player p1, Player p2, Hex robber_pos, int turn) {
    player_one = p1;
    player_two = p2;
    robber_position = robber_pos;
    current_turn = turn;
}

bool GameState::is_game_over() {
    // TODO
    return false;
}

Player GameState::game_winner() {
    // TODO
    return player_one;
}

std::vector<GameState*> GameState::get_all_moves() {
    // TODO
    std::vector<GameState*> all_moves;

    return all_moves;
}

bool GameState::operator==(const GameState& state) const {
    // TODO
    return false;
}

// GAME PLAYER DEFINITIONS

Player::Player(PlayerPolicy *policy) {
    player_policy = policy;
}

bool Player::operator==(const Player& player) const {
    // check all resources same
    for (int i=0; i < NUM_RESOURCES; i++) {
        if (resource_cards[i] != player.resource_cards[i]) return false;
    }

    // check all dev cards same
    for (int i=0; i < NUM_DEVELOPMENT_CARDS; i++) {
        if (dev_cards[i] != player.dev_cards[i]) return false;
    }

    // check all roads same
    if (roads.size() != player.roads.size()) return false;
    for (const auto& road: roads) {
        if (!IS_IN_SET(player.roads, road)) return false;
    }

    // check all settlements same
    if (settlements.size() != player.settlements.size()) return false;
    for (const auto& settlement: settlements) {
        if (!IS_IN_SET(player.settlements, settlement)) return false;
    }

    // check all cities same
    if (cities.size() != player.cities.size()) return false;
    for (const auto& city: cities) {
        if (!IS_IN_SET(player.cities, city)) return false;
    }

    return true;
}

GameState *Player::get_player_move(GameState *game_state) {
    return player_policy->get_best_move(game_state);
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
