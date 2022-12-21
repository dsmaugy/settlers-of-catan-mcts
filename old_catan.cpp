#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"
#include <unordered_set>
#include <algorithm>
#include <random>       // std::default_random_engine

#include <iostream>
#include <cstdio>

#define IS_IN_SET(set, elem) (set.find(elem) != set.end())

int VALUES[19] = {5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11,7}; // 7 isn't a valid reward amt, but is used to randomly assign the desert
int LAND[18] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,4,4,4}; //wheat, wood, wool, ore, brick (-1 = nothing/desert)

int seed = 23445;

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
            } else {
                Hex h;
                if((val = VALUES[v++]) == 7){
                    h = Hex(q,r, -1);   // Desert tile
                    robber_pos = h;
                } else {
                    h = Hex(q,r, LAND[l++]); // Resource tile
                }
                // std::cout << "Hex at (" << h.q << "," << h.r << ") has land value " << h.land_type << " and reward value " << val /*<< ",l=" << l << ", v= << v*/ <<std::endl; 
                tiles.insert(h);
                tile_rewards[h] = val;
            }
        }
        if (q < 0) 
            r1--;
        else 
            r2--;
    }
    //Initialize the player's starter structures, and look-ahead lists for roads and settlements
    Hex p1_a, p1_b, p1_c, p1_d, p2_a, p2_b, p2_c, p2_d;
    for (auto const &h: tiles) {
        if(h.q == -1 && h.r == 1) p1_a = h; 
        if(h.q == -2 && h.r == 1) p1_b = h; 
        if(h.q == -2 && h.r == 2) p1_c = h; 
        if(h.q == -1 && h.r == 0) p1_d = h; 
        if(h.q == 1 && h.r == -1) p2_a = h; 
        if(h.q == 2 && h.r == -1) p2_b = h; 
        if(h.q == 2 && h.r == -2) p2_c = h; 
        if(h.q == 1 && h.r == 0)  p2_d = h; 
    }

    HexIntersection p1_starter = HexIntersection(HexPath(p1_a, p1_b),HexPath(p1_b, p1_c),HexPath(p1_a, p1_c));
    p1.settlements.insert(p1_starter);
    p1.settlement_sites.insert(HexIntersection(HexPath(p1_a, p1_b), HexPath(p1_a, p1_d), HexPath(p1_b, p1_d)));
    p1.roads.insert(HexPath(p1_a, p1_b));
    p1.road_sites.insert(HexPath(p1_a, p1_c));
    p1.road_sites.insert(HexPath(p1_b, p1_c));
    p1.road_sites.insert(HexPath(p1_a, p1_d));
    p1.road_sites.insert(HexPath(p1_b, p1_d));
    p1.victory_points += 1;

    HexIntersection p2_starter = HexIntersection(HexPath(p2_a, p2_b),HexPath(p2_b, p2_c),HexPath(p2_a, p2_c));
    p2.settlements.insert(p2_starter);
    p2.settlement_sites.insert(HexIntersection(HexPath(p2_a, p2_b), HexPath(p2_a, p2_d), HexPath(p2_b, p2_d)));
    p2.roads.insert(HexPath(p2_a, p2_b));
    p2.road_sites.insert(HexPath(p2_b, p2_c));
    p2.road_sites.insert(HexPath(p2_a, p2_c));
    p2.road_sites.insert(HexPath(p2_b, p2_d));
    p2.road_sites.insert(HexPath(p2_a, p2_d));
    p2.victory_points += 1;

    // instantiate the GameState, populate the hex list and map
    game_state = new GameState(p1, p2, robber_pos, 0, 1); //start off with player 1
    GameState::tiles = tiles;
    GameState::tile_rewards = tile_rewards;
}

// random int in range [min,max]
int roll(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

void Game::update_state_with_dice_roll(GameState *state) {
    //roll dice
    int dice = roll(1,6) + roll(1,6);
    // if card counts > 7, randomly take half of the resources
    if (dice == 7){
        state->move_robber = true;  //move the robber
        int removed_cards, resource;

        // update the p1 and p2 card counts
        int p1_cards = 0, p2_cards = 0;
        for (int i=0; i < NUM_RESOURCES; i++) {
            p1_cards += state->player_one.resource_cards[i];
            p2_cards += state->player_two.resource_cards[i];
        }

        state->player_one.card_count = p1_cards;
        state->player_two.card_count = p2_cards;

        // remove from p1
        if (state->player_one.card_count > 7) {
            removed_cards = state->player_one.card_count/2;
            while (removed_cards > 0) {
                resource = roll(0,4); // pick a random kind of card to remove
                if (state->player_one.resource_cards[resource] > 0) {
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
                if (GameState::tile_rewards[hex] == dice) {
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_one.resource_cards[land_type] += 1;
                        state->player_one.card_count += 1;
                    }
                }
            }

        // p1 update cities
        for(const auto& city: state->player_one.cities)
            for (const auto& hex: city.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_one.resource_cards[land_type] += 2;
                        state->player_one.card_count += 2;
                    }
                }
            }

        // p2 update settlements
        for(const auto& settlement: state->player_two.settlements)
            for (const auto& hex: settlement.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_two.resource_cards[land_type] += 1;
                        state->player_two.card_count += 1;
                    }
                }
            }

        // p2 update cities
        for(const auto& city: state->player_two.cities)
            for (const auto& hex: city.adjacent){
                if (GameState::tile_rewards[hex] == dice) {
                    if ((land_type = hex.land_type) != -1 && hex != state->robber_position) {
                        state->player_two.resource_cards[land_type] += 2;
                        state->player_two.card_count += 2;
                    }
                }
            }
    }

}

int Game::next_turn() {
    if (game_state->is_game_over()) {
        if (game_state->game_winner() == game_state->player_one) return 1;
        else return -1;
    }

    // game not over yet

    // roll die
    update_state_with_dice_roll(game_state);

    GameState *new_state;
    if (game_state->current_turn == 0) {
        // std::cout << "Player 1 turn..." << std::endl;
        new_state = game_state->player_one.get_player_move(game_state);
    }
    else {
        // std::cout << "Player 2 turn..." << std::endl;
        new_state = game_state->player_two.get_player_move(game_state);
    }

    delete game_state;
    game_state = new_state;

    // std::cout << "P2 POLICY ADDR: " << game_state->player_two.get_player_move << std::endl;
    // std::cout << "REAL P1 VP: " << game_state->player_one.victory_points << std::endl;
    // std::cout << "REAL P2 VP: " << game_state->player_two.victory_points << std::endl;

    return 0;
}


// GAME STATE DEFINITIONS

GameState::GameState(Player p1, Player p2, Hex robber_pos, int player_turn, int starting_turn_number) {
    player_one = p1;
    player_two = p2;
    robber_position = robber_pos;
    current_turn = player_turn;
    turn_number = starting_turn_number;
}

bool GameState::is_game_over() {
    return (player_one.victory_points >= 10) || (player_two.victory_points >= 10);
    // return false;
}

Player GameState::game_winner() {
    if (!is_game_over()) {
        std::cerr << "Tried to view winner of non-terminal game state" << std::endl;
        exit(1);
    }
    Player winner = (player_one.victory_points >= 20) ? player_one : player_two; 
    return winner;
}

std::vector<GameState*> GameState::get_all_moves() {
    std::vector<GameState*> all_moves;
    std::unordered_set<Hex, HashHex> new_robber_hexes;
    
    int next_turn;
    if (current_turn == 0) next_turn = 1;
    else if (current_turn == 1) next_turn = 0;

    Player new_p1, new_p2, playing;
    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);

    // add the "don't do anything" turn
    all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));

    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);
    playing = (current_turn == 0) ? new_p1 : new_p2;
    // can you cash in resources?
    for(int resource = 0; resource < 5; resource++){
        if (playing.resource_cards[resource] >= 4) {
            playing.resource_cards[resource] -= 4;
            playing.card_count -= 3;
            for (int i = 0; i < 5; i++) {
                if(i == resource) continue;
                playing.resource_cards[i] += 1;
                if (move_robber) {
                    for(auto& pos: new_robber_hexes) all_moves.push_back(new GameState(new_p1, new_p2, pos, next_turn, turn_number+1));
                }else {
                    all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));
                }
                playing.resource_cards[i] -= 1;
            }
                playing.card_count += 3;
            playing.resource_cards[resource] += 4;
        }
    }
    
    // Move the robber
    if(move_robber){
        Player victim;
        if (current_turn == 0) {
            victim = player_two;
        } else {
            victim = player_one;
        }
        // get all of the potential locations
        for(const auto& settlement: victim.settlements)
            for(const auto& hex: settlement.adjacent)
                new_robber_hexes.insert(hex);
        for(const auto& city: victim.cities)
            for(const auto& hex: city.adjacent)
                new_robber_hexes.insert(hex);
    }

    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);
    playing = (current_turn == 0) ? new_p1 : new_p2;
    // can you build a road?
    if (playing.resource_cards[1] >= 2 && playing.resource_cards[4] >= 2) {
        if(playing.road_sites.size() != 0) {
            playing.resource_cards[1] -= 2;
            playing.resource_cards[4] -= 2;
            playing.card_count -= 4;
            for (auto& road: playing.road_sites) {
                Hex h1 = road.hex_one;
                Hex h2 = road.hex_two;
                Hex h3 = Hex(-4,-4, -1);
                Hex h4 = Hex(-4,-4, -1);

                if(road.axis == 0){     //Axis Q
                    // find the neighboring tiles
                    for (auto const& h: GameState::tiles) {
                        if (h.q == h1.q + 1 && h.r == h1.r) h3 = h;
                        if (h.q == h2.q - 1 && h.r == h2.r) h4 = h;
                    }
                } else if (road.axis == 1){ //Axis R
                    // find the neighboring tiles
                    for (auto const& h: GameState::tiles) {
                        if (h.q == h1.q && h.r == h1.r - 1) h3 = h;
                        if (h.q == h2.q && h.r == h2.r + 1) h4 = h;
                    }
                } else if (road.axis == 2) {    //Axis S
                    // find the neighboring tiles
                    for (auto const& h: GameState::tiles) {
                        if (h.q == h2.q && h.r == h2.r - 1) h3 = h;
                        if (h.q == h1.q && h.r == h1.r + 1) h4 = h;
                    }
                }

                bool t1, t2, t3, t4, s1, s2;
                t1 = t2 = t3 = t4 = s1 = s2 = false;

                // check that the road isn't already listed, Ensure that it won't be out of bounds, Try to add.
                if (abs(h3.q) <= 3 && abs(h3.r <=3)) {
                    if ((playing.roads.find(HexPath(h1,h3)) != playing.roads.end())) t1 = playing.road_sites.insert(HexPath(h1,h3)).second;
                    if ((playing.roads.find(HexPath(h2,h3)) != playing.roads.end())) t2 = playing.road_sites.insert(HexPath(h2,h3)).second;
                    s1 = playing.settlement_sites.insert(HexIntersection(HexPath(h1,h2), HexPath(h2,h3), HexPath(h1,h3))).second;
                }
                if (abs(h4.q) <= 3 && abs(h4.r <=3)){
                    if ((playing.roads.find(HexPath(h1,h4)) != playing.roads.end())) t3 = playing.road_sites.insert(HexPath(h1,h4)).second;
                    if ((playing.roads.find(HexPath(h2,h4)) != playing.roads.end())) t4 = playing.road_sites.insert(HexPath(h2,h4)).second;
                    s2 = playing.settlement_sites.insert(HexIntersection(HexPath(h1,h2), HexPath(h1,h4), HexPath(h2,h4))).second;
                }

                // remove the "built" road from the speculative road list and add it to the "built" road list
                playing.roads.insert(road);
                playing.road_sites.erase(road);
                // built road along with wherever the robber is being moved, if we are moving it
                if (move_robber) {
                    for(auto& pos: new_robber_hexes) all_moves.push_back(new GameState(new_p1, new_p2, pos, next_turn, turn_number+1));
                }else {
                    all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));
                }

                // undo all of the changes
                playing.roads.erase(road);
                playing.road_sites.insert(road);

                if (t1) playing.road_sites.erase(HexPath(h1,h3));
                if (t2) playing.road_sites.erase(HexPath(h2,h3));
                if (t3) playing.road_sites.erase(HexPath(h1,h4));
                if (t4) playing.road_sites.erase(HexPath(h2,h4));
                if (s1) playing.settlement_sites.erase(HexIntersection(HexPath(h1,h2), HexPath(h2,h3), HexPath(h1,h3)));
                if (s2) playing.settlement_sites.erase(HexIntersection(HexPath(h1,h2), HexPath(h1,h4), HexPath(h2,h4)));
            }
            playing.resource_cards[1] += 2;
            playing.resource_cards[4] += 2;
            playing.card_count += 4;
        }
    }

    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);
    playing = (current_turn == 0) ? new_p1 : new_p2;
    // can you build a settlement?
    if (playing.resource_cards[0] >= 1 && playing.resource_cards[1] >= 1 && playing.resource_cards[2] >= 1 && playing.resource_cards[4] >= 1) {
        if(playing.settlement_sites.size() != 0) {
            playing.resource_cards[0] -= 1;
            playing.resource_cards[1] -= 1;
            playing.resource_cards[2] -= 1;
            playing.resource_cards[4] -= 1;
            playing.card_count -= 4;
            for (auto& site: playing.settlement_sites) {
                playing.settlement_sites.erase(site);
                playing.settlements.insert(site);
                playing.victory_points += 1;
                if (move_robber) {
                    for(auto& pos: new_robber_hexes) all_moves.push_back(new GameState(new_p1, new_p2, pos, next_turn, turn_number+1));
                }else {
                    all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));
                }
                playing.settlement_sites.insert(site);
                playing.settlements.erase(site);
            }
            playing.resource_cards[0] += 1;
            playing.resource_cards[1] += 1;
            playing.resource_cards[2] += 1;
            playing.resource_cards[4] += 1;
            playing.card_count += 4;
        }
    }

    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);
    playing = (current_turn == 0) ? new_p1 : new_p2;
    // can you make a city?
    if (playing.resource_cards[0] >= 2 && playing.resource_cards[3] >= 3) {
        if(playing.settlement_sites.size() != 0) {
            playing.resource_cards[0] -= 2;
            playing.resource_cards[3] -= 3;
            playing.card_count -= 5;
            for (auto& settlement: playing.settlements) {
                playing.settlements.erase(settlement);
                playing.cities.insert(settlement);
                playing.victory_points += 1;
                if (move_robber) {
                    for(auto& pos: new_robber_hexes) all_moves.push_back(new GameState(new_p1, new_p2, pos, next_turn, turn_number+1));
                }else {
                    all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));
                }
                playing.settlements.insert(settlement);
                playing.cities.erase(settlement);
            }
            playing.resource_cards[0] += 2;
            playing.resource_cards[3] += 3;
            playing.card_count += 5;
        }
    }

    new_p1 = Player(&player_one);
    new_p2 = Player(&player_two);
    playing = (current_turn == 0) ? new_p1 : new_p2;
    // can you buy a victory point?
    if (playing.resource_cards[0] >= 1 && playing.resource_cards[2] >= 1 && playing.resource_cards[3] >= 1) {
            playing.resource_cards[0] -= 1;
            playing.resource_cards[2] -= 1;
            playing.resource_cards[3] -= 1;
            playing.card_count -= 3;
            
            // "buy" the dev card
            playing.victory_points += 1;
            if (move_robber) {
                for(auto& pos: new_robber_hexes) all_moves.push_back(new GameState(new_p1, new_p2, pos, next_turn, turn_number+1));
            }else {
                all_moves.push_back(new GameState(new_p1, new_p2, robber_position, next_turn, turn_number+1));
            }
            playing.resource_cards[0] += 1;
            playing.resource_cards[2] += 1;
            playing.resource_cards[3] += 1;
            playing.card_count += 3;
    }

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

Player::Player(Player *player) {
    // create a new player object from a pre-existing one

    // copy int attributes
    player_policy = player->player_policy;
    card_count = player->card_count;
    victory_points = player->victory_points;

    // copy resources over
    for (int i=0; i < NUM_RESOURCES; i++) resource_cards[i] = player->resource_cards[i];
    for (int i=0; i < NUM_DEVELOPMENT_CARDS; i++) dev_cards[i] = player->dev_cards[i];

    // copy roads/settlements/cities
    for (const auto& road: player->roads) roads.insert(road);
    for (const auto& settlement: player->settlements) settlements.insert(settlement);
    for (const auto& city: player->cities) cities.insert(city);

    for (const auto& road_site: player->road_sites) road_sites.insert(road_site);
    for (const auto& settlement_site: player->settlement_sites) settlement_sites.insert(settlement_site);
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
    
    // check card count
    if (card_count != player.card_count) return false;

    return true;
}

bool GameState::operator==(const GameState& state) const {

    // check if both players are the same
    if (!(player_one == state.player_one)) return false;
    if (!(player_two == state.player_two)) return false;

    // robber on the same position
    if (!(robber_position == state.robber_position)) return false;

    // check turn
    if (!(current_turn == state.current_turn)) return false;
    
    if (turn_number != state.turn_number) return false;

    return true;
}

GameState *Player::get_player_move(GameState *game_state) {
    return player_policy->get_best_move(game_state);
}



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
