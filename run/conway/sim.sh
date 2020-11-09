#!/bin/bash
module load boost

ulimit -n 999999

mpirun --mca mpi_yield_when_idle 1 -oversubscribe -np 386              \
    /home/s9242987/Software/haec-sim/build-phase-profile/main/haec_sim \
    -c ./haec_sim.json -m phase_profile -V info                           \
    /home/tilsche/traces/roco2/charon/lo2s_trace_latest/traces.otf2
