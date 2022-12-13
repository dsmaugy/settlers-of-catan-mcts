CC = g++

CFLAGS = -g -Wall --std="c++17"

run_catan_agent.app: run_catan_agent.o hex.o custom_hash.o
	$(CC) -o $@ $(CFLAGS) $^

run_catan_agent.o: run_catan_agent.cpp custom_hash.h hex.h
	$(CC) -o $@ $(CFLAGS) -c $<

hex.o: hex.cpp custom_hash.h hex.h
	$(CC) -o $@ $(CFLAGS) -c $<

custom_hash.o: custom_hash.cpp custom_hash.h hex.h
	$(CC) -o $@ $(CFLAGS) -c $<

all: run_catan_agent.app

clean:
	rm -f *.app *.o