# P9 "long run"
Experiment to put system under **many levels** of **stable** load.
Achieved by running 1,2,3...44 cores (4 threads each) with one of 7 kernels for 60s each.

Will run for approx. 5h total.

## Building
To run this experiment as designed, these additional components are required:
- [Score-P](https://score-p.org/) installed
- [Score-P ibm powernv plugin](https://github.com/score-p/scorep_plugin_ibmpowernv) in `$LD_LIBRARY_PATH`
- [Score-P metriq plugin](https://github.com/score-p/scorep_plugin_metricq) in `$LD_LIBRARY_PATH`
- `module restore roco2-metricq-ml` must restore the used modules for the build; copy it from this directory: `cp taurus_lmod_list ~/.lmod.d/roco2-metricq-ml`

Recommended way to build, provided you are at the **top-level** directory of roco2:

```
module restore roco2-metricq-ml
mkdir build && cd build
SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler' SCOREP_WRAPPER=off cmake .. -DCMAKE_C_COMPILER=scorep-gcc -DCMAKE_CXX_COMPILER=scorep-g++ -DUSE_SCOREP=ON -DBUILD_TESTING=OFF -DP9_LONGRUN_METRICQ_SERVER='amqps://USER:PASS@HOST'
make SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler'
```

Note that you can use `P9_LONGRUN_METRICQ_SERVER` to specify a default metricq server.

## Running
Running `make` builds an accompanying slurm script, from the build directory it can be launched with:

```
sbatch -A ACCOUNT -- src/configurations/p9_longrun/run_slurm.sh
```

The result will be a directory beginning with `scorep-` and will be placed in your current directory.

The script will try to guess the correct metricq settings for the variables `$SCOREP_METRIC_METRICQ_PLUGIN` and `$SCOREP_METRIC_METRICQ_PLUGIN_SERVER`.
Just set them before invoking the script with `sbatch`, your settings will be kept.

Please note that SLURM sometimes has hiccups and requires additional parameters, especially `--hint=multithread` may be required manually.

Obviously the script is highly dependent on your local deployment, so adjustments may be required.
