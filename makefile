CC = g++

CFLAGS = -g -Wall --std="c++17"

run_catan_agent.app: run_catan_agent.o catan_game_board.o custom_hash.o
	$(CC) -o $@ $(CFLAGS) $^

run_catan_agent.o: run_catan_agent.cpp custom_hash.h catan_game_board.h
	$(CC) -o $@ $(CFLAGS) -c $<

catan_game_board.o: catan_game_board.cpp custom_hash.h catan_game_board.h
	$(CC) -o $@ $(CFLAGS) -c $<

custom_hash.o: custom_hash.cpp custom_hash.h catan_game_board.h
	$(CC) -o $@ $(CFLAGS) -c $<

all: run_catan_agent.app

clean:
	rm -f *.app *.o