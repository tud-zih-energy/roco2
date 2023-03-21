#!/bin/bash

ulimit -n 999999

mpirun --mca mpi_yield_when_idle 1 -oversubscribe -np 101              \
    /home/s9242987/Software/haec-sim/build-lo2s/main/haec_sim \
    -c ./haec_sim.json -m phase_profile -V info                           \
    /home/s9242987/roco2-kallisto/build/src/configurations/kallisto/lo2s_trace_2022-06-23T16-19-08/traces.otf2
