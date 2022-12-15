#include <chrono>
#include <stdlib.h>
#include <climits>

#include "catan_player_policies.h"
#include "catan_game.h"

#define TIME_DIFF_MILLISECONDS(start, end) (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
#define IS_IN_SET(set, elem) (set.find(elem) != set.end())


// RANDOM POLICY
GameState *RandomPolicy::get_best_move(GameState *state) {
    std::vector<GameState*> possible_moves = state->get_all_moves();
    int rand_move_idx = std::rand() % possible_moves.size();

    for (int i=0; i < possible_moves.size(); i++) {
        if (i != rand_move_idx)
            delete possible_moves.at(i);
    }

    return possible_moves.at(rand_move_idx);
}

// MCTS POLICY
MCTSPolicy::MCTSPolicy(int limit, bool parallel) {
    train_time_limit_sec = limit;
    is_parallel = parallel;
}

double MCTSPolicy::get_ucb_value(GameState *parent, GameState* child) {
    // TODO: implement this
    return 0;
}

// TODO: consider breaking this out into another function: traverse_game_tree(GameState *state) that the parallel could use as well?
int MCTSPolicy::update_mcts_serial(GameState *state, bool traverse) {
    // check if terminal state
    if (state->is_game_over()) {
        if (state->game_winner() == state->player_one)
            return 1;
        else
            return -1;
    }

    if (traverse) {
        std::vector<GameState*> possible_moves = state->get_all_moves();
        GameState current_target_child_state;       // will hold best move to make
        double current_target_ucb_value;

        if (state->current_turn == 0)
            current_target_ucb_value = __DBL_MIN__;
        else
            current_target_ucb_value = __DBL_MAX__;

        for (const auto& child_state : possible_moves) {
            MCTS_Edge parent_child_edge = std::make_pair(*state, *child_state);
            if (!IS_IN_SET(edge_map, parent_child_edge)) {
                // encountered a leaf node, start simulating from here

            } else {
                // already seen this node before, see if it's the best we've seen so far
                double current_child_ucb = get_ucb_value(child_state);
                if ((state->current_turn == 0 && current_child_ucb > current_target_ucb_value)
                    || (state->current_turn == 1 && current_child_ucb < current_target_ucb_value)) {
                        current_target_ucb_value = current_child_ucb;
                        current_target_child_state = *child_state;
                }
            }
        }


        // TODO: free all other moves that we're not going down
    } else {
        // simulating tree from here, policy = random
        // play until terminal node
        return update_mcts_serial(default_policy.get_best_move(state), false);
    }

}

int MCTSPolicy::update_mcts_parallel(GameState *state, bool traverse) {
    // TODO: implement
    return 0;
}

GameState *MCTSPolicy::get_best_move(GameState *root_state) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    GameState *current_state = root_state;

    while (TIME_DIFF_MILLISECONDS(start, end) < train_time_limit_sec*1000) {
        // update MCTS for as much as we can here
        if (is_parallel)
            update_mcts_parallel(root_state, true);
        else
            update_mcts_serial(root_state, true);

        end = std::chrono::steady_clock::now();
    }

    // TODO: this should find the best child of root_state from map
    return root_state;
}

// state_map
// edge_map

// CUSTOM HASHING
size_t HashMCTSEdge::operator()(const MCTS_Edge& edge) const {
    // TODO: implement
    return 0;
};