#include <functional>
#include "custom_hash.h"

#include "catan_game_board.h"
#include "catan_game.h"

size_t HashHex::operator()(const Hex& hex) const {
    return (std::hash<int>()(hex.q)
            ^ (std::hash<int>()(hex.r) << 1));
};

size_t HashPath::operator()(const HexPath& road) const {
    return (HashHex()(road.hex_one)
            ^ (HashHex()(road.hex_two) << 1));
};

size_t HashIntersection::operator()(const HexIntersection& building) const {
    size_t hash_value;
    hash_value = (HashPath()(building.path_one)
                    ^ (HashPath()(building.path_two) << 1));
    
    if (building.num_paths == 2) {
        return hash_value;
    } else {
        return (hash_value
                ^(HashPath()(building.path_three) << 1));
    }
};


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

size_t HashMCTSEdge::operator()(const MCTS_Edge& edge) const {
    return 0;
};
