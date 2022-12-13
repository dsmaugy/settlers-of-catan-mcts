CC = g++

CFLAGS = -g -Wall --std="c++17"

run_catan_agent.app: run_catan_agent.o
	$(CC) -o $@ $(CFLAGS) $^

all: run_catan_agent.app

clean:
	rm -f *.app *.o