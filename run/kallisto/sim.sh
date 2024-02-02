#!/bin/bash

ulimit -n 999999

mpirun --mca mpi_yield_when_idle 1 -oversubscribe -np 80              \
    /home/s9242987/Software/haec-sim/build-lo2s/main/haec_sim \
    -c $PWD/haec_sim.json -m phase_profile -V info                           \
    $HOME/roco2/run/kallisto/lo2s_trace_2023-03-23T18-50-52-long/traces.otf2

mpirun --mca mpi_yield_when_idle 1 -oversubscribe -np 80              \
    /home/s9242987/Software/haec-sim/build-lo2s/main/haec_sim \
    -c $PWD/haec_sim_cores.json -m phase_profile -V info                           \
    $HOME/roco2/run/kallisto/lo2s_trace_2023-03-23T18-50-52-long/traces.otf2
