# Kallisto
This configuration executes workloads on the different types of cores of the test alderlake CPU **separately**.

The following CPU "triangles" are tested:

1. all non-HT, then all HT, then all E-cores (each triangle separate from each other)
2. enabling non-HT, HT, E-cores subsequently (in that order, i.e. "one big triangle")
3. enable first non-HT, HT, E-core (all simultaneously), then second non-HT, HT, E-core...

The CPU mapping is non-standard, please refer to the comments in the source file for details.

## Requirements
- Score-P
- cpufreq

## Building

```bash
SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler' SCOREP_WRAPPER=off cmake .. -DCMAKE_C_COMPILER=scorep-gcc -DCMAKE_CXX_COMPILER=scorep-g++ -DUSE_SCOREP=ON -DUSE_FIRESTARTER=OFF
make SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler'
```

## Notes
- The tested frequencies are `3201, 3200, 2500, 1700, 800` MHz.
- According to Linux E- and P- cores support the same frequencies.
- The nominal frequency of the E-cores 2.4 GHz is not available to be selected.

The generated `run.sh` prepares the environment.

You may use the provided `Utility` metric to compare the performance between different runs.
