#include <chrono>
#include <stdlib.h>
#include <climits>
#include <stack>
#include <cmath>
#include <omp.h>
#include <iostream>

#include "catan_player_policies.h"
#include "catan_game.h"

#define TIME_DIFF_MILLISECONDS(start, end) (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
#define IS_IN_SET(set, elem) (set.find(elem) != set.end())

GameState *RandomPolicy::get_best_move(GameState *root_state) {
    std::vector<GameState*> possible_moves = root_state->get_all_moves();
    int rand_move_idx = std::rand() % possible_moves.size();

    // free all other moves
    for (const auto& child_state: possible_moves) {
        if (child_state != possible_moves.at(rand_move_idx)) delete child_state; 
    }

    return possible_moves.at(rand_move_idx);
}

// MCTS POLICY
MCTSPolicy::MCTSPolicy(float limit, bool parallel) {
    train_time_limit_sec = limit;
    is_parallel = parallel;
}

double MCTSPolicy::get_ucb_value(GameState *parent_state, GameState *child_state) {
    MCTS_Edge edge = std::make_pair(*parent_state, *child_state);
    double average_payoff = ((double) state_map.at(*child_state).first) / ((double) state_map.at(*child_state).second);  // use .at to ensure all dict values are init
    double exploration_term = sqrt((2.0*log((double)state_map.at(*parent_state).second))/(double) edge_map.at(edge).second);
    if (parent_state->current_turn == 1) exploration_term *= -1;  // reverse exploration for player 2
    return average_payoff + exploration_term;
}

Reward_Visit_Pair MCTSPolicy::mcts_simulation(GameState *state) {
    int reward = 0;
    int visit = 0;
    RandomPolicy random_picker;
    
    if (is_parallel) {
        #pragma omp parallel default(none) firstprivate(random_picker, state) reduction(+:reward) reduction(+:visit)
        {
            GameState *current_state = state;
            while (!current_state->is_game_over()) {
                current_state = random_picker.get_best_move(current_state);
                if (!current_state->is_game_over())
                    Game::update_state_with_dice_roll(current_state);
            }
            visit = 1;
            if (current_state->game_winner() == current_state->player_one) reward = 1;
            else reward = -1;
        }
    } else {
        GameState *current_state = state;
        while (!current_state->is_game_over()) {
            current_state = random_picker.get_best_move(current_state);
            if (!current_state->is_game_over())
                Game::update_state_with_dice_roll(current_state);

        }
        visit = 1;
        if (current_state->game_winner() == current_state->player_one) reward = 1;
        else reward = -1;
    }
    
    return std::make_pair(reward, visit);
}


GameState *MCTSPolicy::get_best_move(GameState *root_state) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // update the tree for as much time as we can
    while (TIME_DIFF_MILLISECONDS(start, end) < train_time_limit_sec*1000) {
        update_mcts(root_state);
        end = std::chrono::steady_clock::now();
    }

    // from current state, find state with best reward averages
    std::vector<GameState*> possible_moves = root_state->get_all_moves();
    double best_average_value;
    
    if (root_state->current_turn == 0)
        best_average_value = -100000;
    else
        best_average_value = 100000;

    GameState *best_state;

    // int best_child_index = 0, child_index = 0;
    for (const auto& child_state : possible_moves) {
        double child_average = ((double) state_map[*child_state].first) / state_map[*child_state].second;
        if ((root_state->current_turn == 0 && child_average > best_average_value) ||
            (root_state->current_turn == 1 && child_average < best_average_value)) {
            best_average_value = child_average;
            best_state = child_state;
            // best_child_index = child_index;
        }
        // child_index++;
    }

    // free other states
    for (const auto& child_state : possible_moves) {
        if (child_state != best_state) {
            delete child_state;
        } 
    }


    return best_state;
}

void MCTSPolicy::update_mcts(GameState *root_state) {
    std::stack<GameState*> explore_tree_path;
    GameState *current_state;
    
    current_state = root_state;
    explore_tree_path.push(current_state);
    bool encountered_leaf = false;
    while (!current_state->is_game_over() && !encountered_leaf) {
        std::vector<GameState*> possible_moves = current_state->get_all_moves();
        GameState* current_target_child_state = NULL;       // will hold best move to make
        double current_target_ucb_value;

        if (current_state->current_turn == 0)
            current_target_ucb_value = -100000;
        else
            current_target_ucb_value = 100000;


        for (const auto& child_state : possible_moves) {
            MCTS_Edge parent_child_edge = std::make_pair(*current_state, *child_state);
            if (!IS_IN_SET(edge_map, parent_child_edge)) {
                // encountered a leaf node
                current_target_child_state = child_state;
                encountered_leaf = true;
                break;
            } else {
                // already seen this node before, see if it's the best we've seen so far
                double current_child_ucb = get_ucb_value(current_state, child_state);
                if ((current_state->current_turn == 0 && current_child_ucb > current_target_ucb_value)
                    || (current_state->current_turn == 1 && current_child_ucb < current_target_ucb_value)) {
                        current_target_ucb_value = current_child_ucb;
                        current_target_child_state = child_state;
                }
            }
        }

        // go down best UCB child

        // free all other moves
        for (const auto& child_state: possible_moves) {
            if (child_state != current_target_child_state) delete child_state;
        }


            
        explore_tree_path.push(current_target_child_state);
        current_state = current_target_child_state;

        // factor in dice roll with new move
        Game::update_state_with_dice_roll(current_state);
    }

    // at this point *current_state should hold the leaf node, we can start simulating from here
    Reward_Visit_Pair simulation_outcome = mcts_simulation(current_state);

    // propagate the results back up the tree
    // accessing the state/edge map with array notation should initialize values if they don't exist
    // not sure if super safe but whatever
    GameState *backtrack_child_state = explore_tree_path.top();
    state_map[*backtrack_child_state].first += simulation_outcome.first;     // update leaf node manually as we're gonna update by the parent in the loop
    state_map[*backtrack_child_state].second += simulation_outcome.second;

    while(explore_tree_path.size() > 1) {
        // get one more node up the stack
        explore_tree_path.pop();
        GameState *backtrack_parent_state = explore_tree_path.top();

        MCTS_Edge parent_child_edge = std::make_pair(*backtrack_parent_state, *backtrack_child_state);
        state_map[*backtrack_parent_state].first += simulation_outcome.first;
        state_map[*backtrack_parent_state].second += simulation_outcome.second;
        edge_map[parent_child_edge].first += simulation_outcome.first;
        edge_map[parent_child_edge].second += simulation_outcome.second;

        delete backtrack_child_state;

        backtrack_child_state = backtrack_parent_state;
    }

}

// CUSTOM HASHING
size_t HashMCTSEdge::operator()(const MCTS_Edge& edge) const {
    return (HashGameState()(edge.first)
            ^ (HashGameState()(edge.second) << 1));
};