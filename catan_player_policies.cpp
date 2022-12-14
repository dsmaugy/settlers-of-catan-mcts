#include "catan_player_policies.h"
#include "catan_game.h"

MCTSPolicy::MCTSPolicy(int limit, bool parallel) {
    train_time_limit = limit;
    is_parallel = parallel;
}

GameState MCTSPolicy::get_best_move(GameState state) {
    return state;
}