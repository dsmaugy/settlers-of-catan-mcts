#ifndef __PLAYER_POLICIES_GUARD__
#define __PLAYER_POLICIES_GUARD__

#include <unordered_map>

// #include "policy.h"
// #include "catan_game.h"
#include "custom_hash.h"
// #include "catan_game_board.h"


typedef std::pair<int, int> Reward_Visit_Pair;
typedef std::pair<GameState, GameState> MCTS_Edge;

// forward declarations
// class GameState;

class PlayerPolicy {
    public:
        GameState get_best_move(GameState state);
};

class RandomPolicy: public PlayerPolicy {
    public:
        GameState get_best_move(GameState state);
};

class MCTSPolicy: public PlayerPolicy {
    public:
        int train_time_limit_sec;
        bool is_parallel;

        MCTSPolicy(int limit, bool parallel);
        GameState get_best_move(GameState root_state);
        GameState mcts_search_serial(GameState root_state);
        GameState mcts_search_parallel(GameState root_state);

    private:
        // std::unordered_map<GameState, Reward_Visit_Pair, HashGameState> state_map;
        // std::unordered_map<MCTS_Edge, Reward_Visit_Pair, HashMCTSEdge> edge_map;                  // TODO: make a hash function for this
        // std::unordered_map<int, Reward_Visit_Pair> edge_map;                  // TODO:delete
        double get_ucb_value(GameState state);
        RandomPolicy default_policy;
        int update_mcts_serial(GameState state, bool traverse);
        int update_mcts_parallel(GameState state, bool traverse);


};

#endif