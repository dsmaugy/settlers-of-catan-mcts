#!/bin/bash

#SBATCH --reservation=cpsc424
#SBATCH --cpus-per-task=20
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=4:00:00
#SBATCH --mem-per-cpu=5000
#SBATCH --job-name=catan-out
#SBATCH --output=%x-%j.out


module load intel

make clean
make

export OMP_NUM_THREADS=4
./run_catan_agent.app