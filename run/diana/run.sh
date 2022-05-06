#!/bin/bash
module load lo2s/2019-07-10
module load scorep_metricq
module load mkl

export SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT="24h"
export X86_ENERGY_SOURCE=x86a-rapl

cd /home/tilsche/roco2/build_diana/src/configurations/diana

make -j || exit 1

sudo perf probe -d roco2:metrics
sudo perf probe -x ./roco2_diana roco2:metrics=_ZN5roco27metrics4meta5writeEmmlm experiment frequency shell threads || exit 1
LD_PRELOAD=/opt/global/18.04/fftw/3.3.8/lib/libfftw3.so GOMP_CPU_AFFINITY=0-71 lo2s -X -t roco2:metrics -o "/fastfs/tilsche/roco2/diana/lo2s_trace_{DATE}" ./roco2_diana

