#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"
#include <unordered_set>
#include <algorithm>
#include <random>       // std::default_random_engine

#include <iostream>

#define IS_IN_SET(set, elem) (set.find(elem) != set.end())

int VALUES[19] = {5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11,7}; // 7 isn't a valid reward amt, but is used to randomly assign the desert
int LAND[18] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,4,4,4}; //wheat, wood, wool, ore, brick (-1 = nothing/desert)

//TODO: Better seed?
int seed = 12345;

// GameState::unordered_set<Hex, HashHex> init_set(unordered_set<Hex,HashHex> tiles){
//     GameState::tiles = tiles;
// }

std::unordered_set<Hex, HashHex> GameState::tiles;
std::unordered_map<Hex, int, HashHex> GameState::tile_rewards;



// Initialize the game board (Hexes), initial game state?
Game::Game(Player p1, Player p2) {
    // Create the board tiles
    std::unordered_set<Hex, HashHex> tiles;
    std::unordered_map<Hex, int, HashHex> tile_rewards;

    std::shuffle(std::begin(VALUES), std::end(VALUES), std::default_random_engine(seed));
    std::shuffle(std::begin(LAND), std::end(LAND), std::default_random_engine(seed));

    int r1 = 0, r2 = 3, v = 0, l = 0;
    int val;

    Hex robber_pos;

    for (int q = -3; q <= 3; q++) {
        for (int r = r1; r <= r2; r++) {
            // if out of bounds
            if (abs(q) == 3 || abs(r) == 3 || abs(-q-r) == 3) {
                tiles.insert(Hex(q,r, -1));
                // std::cout << "Hex at (" << q << "," << r << ") has land value " << -1 << " and reward value -----" <<std::endl; 

            } else {
                Hex h;
        
                if((val = VALUES[v++]) == 7){
                    h = Hex(q,r, -1);   // Desert tile
                    robber_pos = h;
                } else {
                    h = Hex(q,r, LAND[l++]);
                }
                std::cout << "Hex at (" << h.q << "," << h.r << ") has land value " << h.land_type << " and reward value " << val /*<< ",l=" << l << ", v=" << v*/ <<std::endl; 
                tiles.insert(h);
                tile_rewards[h] = val;
            }
        }
        if (q < 0) 
            r1--;
        else 
            r2--;
    }
    //TODO: set the initial player settlements here? Figure out where it is that they'll go
    
    // instantiate the GameState, populate the hex list and map
    game_state = GameState(p1, p2, robber_pos, 1);
    GameState::tiles = tiles;
    GameState::tile_rewards = tile_rewards;
}

// random int in range [min,max]
int roll(int min, int max) {
    return rand() % (min - max + 1) + min;
}

void Game::update_state_with_dice_roll(GameState *state) {
    // roll dice
    //TODO: Better rice rolling algorithm? currently starting w 7
    int dice = roll(1,6) + roll(1,6);
    // std::cout << "Dice roll: " << dice << std::endl;

    if (dice == 7){
        state->move_robber = true;
        int removed_cards, resource;
        // if card counts > 7, randomly take half of the resources
        // remove from p1
        if (state->player_one.card_count > 7) {
            // std::cout << "ok" <<std::endl;
            removed_cards = state->player_one.card_count/2;
            while (removed_cards > 0) {
                // TODO: This also seems a little biased to [0-2]
                resource = roll(0,4); // pick a random kind of card to remove
                // std::cout << "resource=" << resource << std::endl;
                if (state->player_one.resource_cards[resource] > 0) {
                    // std::cout << "removed" << std::endl;
                    state->player_one.resource_cards[resource]--;
                    state->player_one.card_count--;
                    removed_cards--;
                }
            }
        }
        //remove from p2
        if (state->player_two.card_count > 7) {
            removed_cards = state->player_two.card_count/2;
            while (removed_cards > 0) {
                resource = roll(0,4); // pick a random kind of card to remove
                if (state->player_two.resource_cards[resource] > 0) {
                    state->player_two.resource_cards[resource]--;
                    state->player_two.card_count--;
                    removed_cards--;
                }
            }
        }

    } else {
        int land_type;
        // p1 update settlements
        for(const auto& settlement: state->player_one.settlements)
            for (const auto& hex: settlement.adjacent){
                // std::cout << "hex: " << hex.q << hex.r << hex.land_type;
                if (GameState::tile_rewards[hex] == dice) {
                    // std::cout << "state->player_one hit!" << std::endl;
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_one.resource_cards[land_type] += 1;
                        state->player_one.card_count += 1;
                    }
                }
            }

        // state->player_one update cities
        for(const auto& city: state->player_one.cities)
            for (const auto& hex: city.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    // std::cout << "state->player_one city hit!" << std::endl;
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        // std::cout << "state->player_one city valid!" << std::endl;
                        // std::cout << "state->player_one resource r["<< land_type << "]=" <<state->player_one.resource_cards[land_type];
                        state->player_one.resource_cards[land_type] += 2;
                        // std::cout << "state->player_one resource r["<< land_type << "]=" <<state->player_one.resource_cards[land_type];
                        state->player_one.card_count += 2;
                    }
                }
            }

        // state->player_two update settlements
        for(const auto& settlement: state->player_two.settlements)
            for (const auto& hex: settlement.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    // std::cout << "state->player_two hit!" << std::endl;
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_two.resource_cards[land_type] += 1;
                        state->player_two.card_count += 1;
                    }
                }
            }

        // state->player_two update cities
        for(const auto& city: state->player_two.cities)
            for (const auto& hex: city.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    // std::cout << "state->player_two city hit!" << std::endl;
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        // std::cout << "state->player_two city valid!" << std:xx/:endl;
                        state->player_two.resource_cards[land_type] += 2;
                        state->player_two.card_count += 2;
                    }
                }
            }
    }
    // std::cout << "hex: q=" << hex.q << ", r=" << hex.r << std::endl;
}


// GAME STATE DEFINITIONS

GameState::GameState(Player p1, Player p2, Hex robber_pos, int turn) {
    player_one = p1;
    player_two = p2;
    robber_position = robber_pos;
    current_turn = turn;
}

// TODO: ensure that this works
bool GameState::is_game_over() {
    return (player_one.victory_points == 10) || (player_two.victory_points == 10);
    // return false;
}

// TODO: ensure that this works
Player GameState::game_winner() {
    Player winner = (player_one.victory_points == 10) ? player_one : player_two; 
    return winner;
}

std::vector<GameState*> GameState::get_all_moves() {
    // TODO: 
    std::vector<GameState*> all_moves;

    // Implementation restrictions: 
    //   - You can build ONE thing
    //   - You can use ONE card
    
    return all_moves;
}



// GAME PLAYER DEFINITIONS

Player::Player(PlayerPolicy *policy) {
    player_policy = policy;

    // init resource cards and dev values
    for (int i = 0; i < NUM_DEVELOPMENT_CARDS; i++) dev_cards[i] = 0;
    for (int i = 0; i < NUM_RESOURCES; i++) resource_cards[i] = 0;

    // init victory points, card count
    victory_points = 0;
    card_count = 0;
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

    // check points count
    if (victory_points != player.victory_points) return false;

    return true;
}

// TODO: ensure that this works
bool GameState::operator==(const GameState& state) const {

    // check if both players are the same
    if (!(player_one == state.player_one)) return false;
    if (!(player_two == state.player_two)) return false;

    // robber on the same position
    if (!(robber_position == state.robber_position)) return false;

    // check turn
    if (!(current_turn == state.current_turn)) return false;

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
