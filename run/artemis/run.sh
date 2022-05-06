#!/bin/bash
#module load lo2s/2019-05-06
module load lo2s
module load scorep_metricq
module load mkl
module load gcc

export SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT="24h"
export SCOREP_METRIC_METRICQ_PLUGIN="elab.artemis.power,elab.ariel.s0.package.power.100Hz,elab.ariel.s1.package.power.100Hz,elab.ariel.s0.dram.power.100Hz,elab.ariel.s1.dram.power.100Hz"

cd /home/tilsche/roco2/build_artemis/src/configurations/artemis

make -j || exit 1

sudo perf probe -d roco2:metrics
sudo perf probe -x ./roco2_artemis roco2:metrics=_ZN5roco27metrics4meta5writeEmmlm experiment frequency shell threads || exit 1
LD_PRELOAD=/opt/global/18.04/fftw/3.3.8/lib/libfftw3.so GOMP_CPU_AFFINITY=0-71 lo2s -X -t roco2:metrics -o "/fastfs/tilsche/roco2/artemis/lo2s_trace_{DATE}" ./roco2_artemis
