#include <chrono>

#include "catan_player_policies.h"
#include "catan_game.h"

#define TIME_DIFF_MILLISECONDS(start, end) (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())

MCTSPolicy::MCTSPolicy(int limit, bool parallel) {
    train_time_limit_sec = limit;
    is_parallel = parallel;
}

GameState MCTSPolicy::get_best_move(GameState state) {
    if (is_parallel)
        return mcts_search_parallel(state);
    else
        return mcts_search_serial(state);
}

GameState MCTSPolicy::mcts_search_serial(GameState root_state) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    while (TIME_DIFF_MILLISECONDS(start, end) < train_time_limit_sec*1000) {
        // update MCTS for as much as we can here
        end = std::chrono::steady_clock::now();
    }
}