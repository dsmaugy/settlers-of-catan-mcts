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
        if (child_state != possible_moves.at(rand_move_idx)) delete child_state; // TODO: maybe want to verify if this actually works with print debug
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
    // std::cout << "starting simulation" << std::endl;
    
    // TODO needs verification
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
        }
    } else {
        GameState *current_state = state;
        while (!current_state->is_game_over()) {
            current_state = random_picker.get_best_move(current_state);
            // std::cout << "Random State: " << current_state->player_one.resource_cards[0] << std::endl;
            if (!current_state->is_game_over())
                Game::update_state_with_dice_roll(current_state);
            // exit(0);

        }
        // std::cout << "done!!!!" << std::endl;
        visit = 1;
        if (current_state->game_winner() == current_state->player_one) reward = 1;
        else reward = -1;
    }
    
    // std::cout << "done simulation" << std::endl;
    return std::make_pair(reward, visit);
}


GameState *MCTSPolicy::get_best_move(GameState *root_state) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "gonna start updating mcts" << std::endl;
    // update the tree for as much time as we can
    while (TIME_DIFF_MILLISECONDS(start, end) < train_time_limit_sec*1000) {
        update_mcts(root_state);
        end = std::chrono::steady_clock::now();
        // std::cout << "TIME: " << TIME_DIFF_MILLISECONDS(start, end) << " / " << train_time_limit_sec*1000 << std::endl;
    }

    std::cout << "done updating mcts, now returning best move" << std::endl;
    // from current state, find state with best reward averages
    std::vector<GameState*> possible_moves = root_state->get_all_moves();
    double best_average_value;
    
    if (root_state->current_turn == 0)
        best_average_value = -100000;
    else
        best_average_value = 100000;

    GameState *best_state;

    for (const auto& child_state : possible_moves) {
        double child_average = ((double) state_map[*child_state].first) / state_map[*child_state].second;
        std::cout << "Reward: " << state_map[*child_state].first << " Visit: " << state_map[*child_state].second << std::endl;
        if ((root_state->current_turn == 0 && child_average > best_average_value) ||
            (root_state->current_turn == 1 && child_average < best_average_value)) {
            best_average_value = child_average;
            best_state = child_state;
        }
    }
    if (root_state->current_turn == 0)
        std::cout << "BEST STATE # roads: " << best_state->player_one.roads.size() << " val: " << best_average_value << std::endl;

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
    // std::cout << "starting tree policy loop" << std::endl;
    while (!current_state->is_game_over() && !encountered_leaf) {
        // std::cout << "top of the tree policy loop" << std::endl;
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
                // explore_tree_path.push(child_state);
                current_target_child_state = child_state;
                encountered_leaf = true;
                // std::cout << "BRUHH" << std::endl;
                break;
            } else {
                // already seen this node before, see if it's the best we've seen so far
                double current_child_ucb = get_ucb_value(current_state, child_state);
                // std::cout << "UCB: " << current_child_ucb << " Comparing Against: " << current_target_ucb_value << std::endl;
                // std::cout << (current_child_ucb > current_target_ucb_value) << std::endl;
                if ((current_state->current_turn == 0 && current_child_ucb > current_target_ucb_value)
                    || (current_state->current_turn == 1 && current_child_ucb < current_target_ucb_value)) {
                        current_target_ucb_value = current_child_ucb;
                        current_target_child_state = child_state;
                        // std::cout << "made it in" << std::endl;
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

    // std::cout << "got out of tree policy loop" << std::endl;
    // at this point *current_state should hold the leaf node, we can start simulating from here
    Reward_Visit_Pair simulation_outcome = mcts_simulation(current_state);

    // propagate the results back up the tree
    // accessing the state/edge map with array notation should initialize values if they don't exist
    // not sure if super safe but whatever
    GameState *backtrack_child_state = explore_tree_path.top();
    state_map[*backtrack_child_state].first += simulation_outcome.first;     // update leaf node manually as we're gonna update by the parent in the loop
    state_map[*backtrack_child_state].second += simulation_outcome.second;

    // std::cout << "propagating stats back up tree (size=" << explore_tree_path.size() << ")" << std::endl;
    while(explore_tree_path.size() > 1) {
        // get one more node up the stack
        // std::cout << "top of while loop" << std::endl;
        explore_tree_path.pop();
        GameState *backtrack_parent_state = explore_tree_path.top();

        // std::cout << "got backtrack parent" << std::endl;
        MCTS_Edge parent_child_edge = std::make_pair(*backtrack_parent_state, *backtrack_child_state);
        state_map[*backtrack_parent_state].first += simulation_outcome.first;
        state_map[*backtrack_parent_state].second += simulation_outcome.second;
        edge_map[parent_child_edge].first += simulation_outcome.first;
        edge_map[parent_child_edge].second += simulation_outcome.second;

        // std::cout << "gonna delete " << backtrack_child_state << std::endl;
        delete backtrack_child_state;
        // std::cout << "deleted child" << std::endl;
        backtrack_child_state = backtrack_parent_state;
    }



    // std::cout << "done propagating" << std::endl;

}

// CUSTOM HASHING
size_t HashMCTSEdge::operator()(const MCTS_Edge& edge) const {
    return (HashGameState()(edge.first)
            ^ (HashGameState()(edge.second) << 1));
};