#!/bin/bash

#SBATCH --reservation=cpsc424
#SBATCH --cpus-per-task=20
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=4:00:00
#SBATCH --mem-per-cpu=5000
#SBATCH --job-name=catan-out
#SBATCH --output=%x-%j.out

# comment me out if running locally
module load intel

make clean
make

./run_catan_agent.app mcts-serial random 0.2

echo ""

export OMP_NUM_THREADS=4
echo "Running with 4 threads against RANDOM"
./run_catan_agent.app mcts-parallel random 0.2
./run_catan_agent.app mcts-parallel random 0.4
./run_catan_agent.app mcts-parallel random 0.6

echo ""

echo "Running with 4 threads"
export OMP_NUM_THREADS=4
./run_catan_agent.app mcts-serial mcts-parallel 0.2
./run_catan_agent.app mcts-serial mcts-parallel 0.4
./run_catan_agent.app mcts-serial mcts-parallel 0.6

echo ""

echo "Running with 6 Threads"
export OMP_NUM_THREADS=6
./run_catan_agent.app mcts-serial mcts-parallel 0.2
./run_catan_agent.app mcts-serial mcts-parallel 0.4
./run_catan_agent.app mcts-serial mcts-parallel 0.6

echo ""

echo "Running with 8 Threads"
export OMP_NUM_THREADS=8
./run_catan_agent.app mcts-serial mcts-parallel 0.2
./run_catan_agent.app mcts-serial mcts-parallel 0.4
./run_catan_agent.app mcts-serial mcts-parallel 0.6

echo ""

echo "Running with 20 Threads"
export OMP_NUM_THREADS=20
./run_catan_agent.app mcts-serial mcts-parallel 0.2
./run_catan_agent.app mcts-serial mcts-parallel 0.4
./run_catan_agent.app mcts-serial mcts-parallel 0.6