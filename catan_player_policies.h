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
        // get a random next move
        GameState *get_best_move(GameState *state);
};

class MCTSPolicy: public PlayerPolicy {
    public:
        int train_time_limit_sec;
        bool is_parallel;

        MCTSPolicy(int limit, bool parallel);

        // get the best move after performing MCTS from current state for the allotted time limit
        GameState *get_best_move(GameState *state);

    private:
        // mapping between game states and rewards/visits for the game state node
        std::unordered_map<GameState, Reward_Visit_Pair, HashGameState> state_map;

        // mapping between game state transitions and rewards/visits for the transition
        std::unordered_map<MCTS_Edge, Reward_Visit_Pair, HashMCTSEdge> edge_map; 

        // get the UCB2 value of a particular transition
        double get_ucb_value(GameState *parent, GameState *child);

        // policy used to simulate playing the game tree
        RandomPolicy default_policy;

        // update the tree in serial
        int update_mcts_serial(GameState *state, bool traverse);

        // update the tree in parallel
        int update_mcts_parallel(GameState *state, bool traverse);


};

#endif