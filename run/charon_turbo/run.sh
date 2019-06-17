#!/bin/bash
source /etc/profile.d/lmod.sh
source /etc/profile.d/zih-a-lmod.sh
module load lo2s
module load scorep_metricq
module load mkl

export SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT=24h
export LO2S_OUTPUT_LINK=/home/tilsche/traces/roco2/charon_turbo/lo2s_trace_latest

cd /home/tilsche/roco2/build/src/configurations/charon_turbo

make -j || exit 1

ulimit -n 999999

elab frequency nominal

perf probe -d roco2:metrics
perf probe -x ./roco2_charon_turbo roco2:metrics=_ZN5roco27metrics4meta5writeEmmlm experiment frequency shell threads || exit 1
LD_PRELOAD=/opt/global/18.04/fftw/3.3.8/lib/libfftw3.so GOMP_CPU_AFFINITY=0-71 lo2s \
-X -E msr/aperf/ -E msr/mperf/ -t roco2:metrics -o '/home/tilsche/traces/roco2/charon_turbo/lo2s_trace_{DATE}' ./roco2_charon_turbo
