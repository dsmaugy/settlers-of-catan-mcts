CC=g++
CPPFLAGS=-g -Wall -fopenmp --std="c++17" 

run_catan_agent.app: run_catan_agent.o catan_game.o catan_game_board.o # catan_player_policies.o
	$(CC) -o $@ $(CPPFLAGS) $^

all: run_catan_agent.app

clean:
	rm -f *.app *.o