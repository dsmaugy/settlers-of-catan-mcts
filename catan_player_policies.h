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

    private:
        std::unordered_map<GameState, Reward_Visit_Pair, HashGameState> state_map;
        std::unordered_map<MCTS_Edge, Reward_Visit_Pair, HashMCTSEdge> edge_map;                  // TODO: make a hash function for this
        double get_ucb_value(GameState *parent_state, GameState *child_state);
        RandomPolicy default_policy;
        void update_mcts(GameState *state);
        // int update_mcts_parallel(GameState *state, bool traverse);

        // simulate a single serial playout to terminal state. Return pair will either be <0, 1> or <1, 1>
        Reward_Visit_Pair mcts_simulation_serial(GameState *state); 

        // simulate multiple playouts to terminal state in parallel. 
        // Return pair should hold playout stats for all simulations in this group.
        Reward_Visit_Pair mcts_simulation_parallel(GameState *state);

};



#endif