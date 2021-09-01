# P9 Highlow
Helper to create aliasing inside P9 OCC.

Switches between 50% busy wait and 50% sleep per cycle.

## Building
To run this experiment as designed, these additional components are required:
- [Score-P](https://score-p.org/) installed
- [Score-P ibm powernv plugin](https://github.com/score-p/scorep_plugin_ibmpowernv) in `$LD_LIBRARY_PATH`
- `module restore roco2-ml` must restore the used modules for the build
    get it from this directory: `cp taurus_lmod_list ~/.lmod.d/`

Recommended way to build, provided you are at the **top-level** directory of roco2:

```
module restore roco2-ml
mkdir build && cd build
SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler' SCOREP_WRAPPER=off cmake .. -DCMAKE_C_COMPILER=scorep-gcc -DCMAKE_CXX_COMPILER=scorep-g++ -DUSE_SCOREP=ON -DBUILD_TESTING=OFF -DROCO2_HIGHLOW_INSTRUMENT_PHASES=OFF -DP9_HIGHLOW_FREQS=2000+-5
make SCOREP_WRAPPER_INSTRUMENTER_FLAGS='--user --openmp --thread=omp --nocompiler'
```

Adjust the last two parameters as needed.
Setting `ROCO2_HIGHLOW_INSTRUMENT_PHASES` to `ON` will result in **huge** traces (multiple GB compared to a few KB otherwise) at high frequencies. 
At low frequencies it does not matter.

### Frequencies
"1 Hz" denotes one full cycle of 500ms busywait and 500ms sleep per second.
To set frequencies compile with `P9_HIGHLOW_FREQS` set to a frequency list as follows:

any of those separated by commas:
- direct frequency (`0.2`)
- range (`78-80`, only integers)
- range with plusminus (`2000+-5`, only integers)

spaces are ignored

## Running
Running `make` builds an accompanying slurm script, from the build directory it can be launched with:

```
sbatch -A ACCOUNT -- src/configurations/p9_highlow/run_slurm.sh
```

The result will be a directory beginning with `scorep-` and will be placed in your current directory.

Please note that SLURM sometimes has hiccups and requires additional parameters, especially `--hint=multithread` maybe must be set manually.
The default maximum runtime is set to 1:59:59, keep in mind that only roughly 25-50% of runtime are alotted to the experiment, the remainder is Score-P postprocessing.

Obviously the script is highly dependent on your local deployment, so adjustments may be required.
