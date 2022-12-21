#include <iostream>
#include <unordered_map>
#include <string.h>


#include "omp.h"
#include "catan_game.h"
#include "catan_player_policies.h"

#include <chrono>

const char *CMD_LINE_ERROR = "Invalid command line arguments. Usage: ./run_catan_agent <mcts-serial|mcts-parallel|random> <mcts-serial|mcts-parallel|random> <mcts time limit>";

int main(int argc, char** argv) {
    std::cout << "hello world" << "df\n";

    // // args: p1_model p2_model time_limit num_games

    // // GameState::tiles = unordered_set<Hex> tiles;
    // // GameState::tile_rewards = unordered_map<Hex> tile_rewards;

    Player one = Player();
    Player two = Player();

    Game game = Game(Player(), Player());
    // Game game = Game(one, two);

    // // std::cout << "Settlements here:" << game.game_state.player_one.settlements.size() << std::endl;
    // // std::cout << "Settlements here:" << game.game_state.player_two.settlements.size() << std::endl;

    // Hex set_test = Hex(-2,0,2);
    // Hex set_test2 = Hex(1,0,3);
    // std::cout << "GameState set/map worked, since set_test has value " << GameState::tile_rewards[set_test] << std::endl;
    // std::cout << "GameState set/map worked, since set_test2 has value " << GameState::tile_rewards[set_test2] << std::endl;
    
    // get command line args
    char *p1_policy_arg, *p2_policy_arg;
    float time_limit;
    int num_games;

    if (argc != 5) {
        std::cerr << CMD_LINE_ERROR << std::endl;
        exit(-1);
    }

    PlayerPolicy *p1_policy, *p2_policy;
    Player p1, p2;
    Game catan_game;

    p1_policy_arg = argv[1];
    p2_policy_arg = argv[2];
    time_limit = atof(argv[3]);
    num_games = atoi(argv[4]);

    if (strcmp(p1_policy_arg, "mcts-serial") == 0) {
        p1_policy = new MCTSPolicy(time_limit, false);
    } else if (strcmp(p1_policy_arg, "mcts-parallel") == 0) {
        p1_policy = new MCTSPolicy(time_limit, true);
    } else if (strcmp(p1_policy_arg, "random") == 0) {
        p1_policy = new RandomPolicy();
    } else {
        std::cerr << CMD_LINE_ERROR << std::endl;
        exit(-1);
    }

    if (strcmp(p2_policy_arg, "mcts-serial") == 0) {
        p2_policy = new MCTSPolicy(time_limit, false);
    } else if (strcmp(p2_policy_arg, "mcts-parallel") == 0) {
        p2_policy = new MCTSPolicy(time_limit, true);
    } else if (strcmp(p2_policy_arg, "random") == 0) {
        p2_policy = new RandomPolicy();
    } else {
        std::cerr << CMD_LINE_ERROR << std::endl;
        exit(-1);
    }

    #pragma omp parallel
    {
        if (omp_get_thread_num() == 0) 
        std::cout << "Running CATAN agent with " << omp_get_num_threads() << " threads." << std::endl;
    }

    
    p1 = Player(p1_policy);
    p2 = Player(p2_policy);
    catan_game = Game(p1, p2);

    int game_status = catan_game.next_turn();
    while (game_status == 0)
        catan_game.next_turn();

    if (game_status == 1)
        std::cout << "Game ended... Player 1 won!" << std::endl;
    else
        std::cout << "Game ended... Player 2 won!" << std::endl;
   
    
    return 0;
}