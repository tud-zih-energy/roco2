# roco2
A synthetic workload generator written in C++ with integrated [Score-P](http://www.vi-hps.org/projects/score-p) instrumentation.

## Getting started

The folder `src/configurations/example` contains one example configuration.
You can build this example, however it will require small adjustments to fit your machine. (See [Building](#user-content-building))

The file `src/configurations/example/experiment.cpp` contains all configuration,
which is necessary to adapt to your machine. For simple adjustments, you can edit the section ```EDIT GENERIC SETTINGS```. This section comprises the variables representing the duration of one experiment, a list of all tested frequencies and T-states, and the patterns of the active hardware threads. These four knobs determine the length of one execution.
For more fine grained changes to the execution, you can edit the section ```EDIT TASK PLAN```. This section determines the order of execution for the different workload kernels.

For further details, refer to the [Wiki](https://github.com/tud-zih-energy/roco2/wiki).

## Dependencies
- Mandatory
    - A compiler with C++14 and OpenMP support
    - A recent Linux kernel
    - Intel MKL or CBLAS
    - libnuma
- Optional
    - [Score-P](http://www.vi-hps.org/projects/score-p)
    - libcpufreq
    - [X86Adapt](https://github.com/tud-zih-energy/x86_adapt)

The build will try to detect the presence or absence of these dependencies.
To enforce their usage set the corresponding options, e.g. by executing `ccmake .` in the build directory.

## Building
This project uses the standard build process for CMake projects.
Note that the configuration step will try to initialize the git submodules.

```
git clone https://github.com/tud-zih-energy/roco2.git
cd roco2
mkdir build && cd build
cmake ..
make
```

## Acknowledgments

This work is based on "[Hackenberg et. al.: Power measurement techniques on standard compute nodes: A quantitative comparison](http://ieeexplore.ieee.org/abstract/document/6557170/)".
