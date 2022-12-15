#ifndef __PLAYER_POLICIES_GUARD__
#define __PLAYER_POLICIES_GUARD__
#include <cstddef>
#include <unordered_map>

#include "policy.h"
#include "catan_game.h"


typedef std::pair<int, int> Reward_Visit_Pair;
typedef std::pair<GameState, GameState> MCTS_Edge;

class HashMCTSEdge {
    public:
        size_t operator()(const MCTS_Edge& edge) const;
};

class RandomPolicy: public PlayerPolicy {
    public:
        GameState *get_best_move(GameState *state);
};

class MCTSPolicy: public PlayerPolicy {
    public:
        int train_time_limit_sec;
        bool is_parallel;

        MCTSPolicy(int limit, bool parallel);
        GameState *get_best_move(GameState *state);
        GameState *mcts_search_serial(GameState *root_state);
        GameState *mcts_search_parallel(GameState *root_state);

    private:
        std::unordered_map<GameState, Reward_Visit_Pair, HashGameState> state_map;
        std::unordered_map<MCTS_Edge, Reward_Visit_Pair, HashMCTSEdge> edge_map;                  // TODO: make a hash function for this
        double get_ucb_value(GameState *state);
        RandomPolicy default_policy;
        int update_mcts_serial(GameState *state, bool traverse);
        int update_mcts_parallel(GameState *state, bool traverse);


};



#endif