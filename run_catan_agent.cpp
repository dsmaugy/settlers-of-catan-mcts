#include <iostream>
#include <unordered_map>
#include <string.h>


#include "omp.h"
#include "catan_game.h"
#include "catan_player_policies.h"

#include <chrono>

const char *CMD_LINE_ERROR = "Invalid command line arguments. Usage: ./run_catan_agent <mcts-serial|mcts-parallel|random> <mcts-serial|mcts-parallel|random> <mcts time limit>";

int main(int argc, char** argv) {

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


     
    int p1_wins = 0;
    for (int i=0; i < num_games; i++) {
        p1 = Player(p1_policy);
        p2 = Player(p2_policy);
        catan_game = Game(p1, p2);
        int game_status = catan_game.next_turn();
        int turn = 1;
        while (game_status == 0) {
            std::cout << "GAME TURN #: " << turn << std::endl;
            game_status = catan_game.next_turn();
            turn++;
        }
            

        if (game_status == 1)
            std::cout << "Game ended... Player 1 won!" << std::endl;
        else
            std::cout << "Game ended... Player 2 won!" << std::endl;
    }

    std::cout << "P1 Winning Pct: " << (double)p1_wins/(double)num_games << std::endl; 
   
    
    return 0;
}