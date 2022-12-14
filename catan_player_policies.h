#ifndef __PLAYER_POLICIES_GUARD__
#define __PLAYER_POLICIES_GUARD__

#include <unordered_map>

#include "policy.h"
#include "custom_hash.h"

typedef std::pair<int, int> mcts_node;

class MCTSPolicy: public PlayerPolicy {
    public:
        int train_time_limit_sec;
        bool is_parallel;

        MCTSPolicy(int limit, bool parallel);
        GameState get_best_move(GameState state);
        GameState mcts_search_serial(GameState state);
        GameState mcts_search_parallel(GameState state);

    private:
        std::unordered_map<GameState, mcts_node, HashGameState> state_map;

};

class RandomPolicy: public PlayerPolicy {
    GameState get_best_move(GameState state);
};

#endif