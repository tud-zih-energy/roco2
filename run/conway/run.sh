#!/bin/bash
source /etc/profile.d/lmod.sh
source /etc/profile.d/zih-a-lmod.sh
module load lo2s
module load scorep_metricq
module load blis

export SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT=24h
export LO2S_OUTPUT_LINK=/home/s9242987/traces/roco2/conway/lo2s_trace_latest

cd /home/s9242987/roco2/build/src/configurations/conway

make -j || exit 1

ulimit -n 999999

elab frequency nominal

perf probe -d roco2:metrics
perf probe -x ./roco2_charon roco2:metrics=_ZN5roco27metrics4meta5writeEmmlm experiment frequency shell threads || exit 1
GOMP_CPU_AFFINITY=0-63 lo2s \
-X --standard-metrics -t roco2:metrics -o '/home/tilsche/traces/roco2/conway/lo2s_trace_{DATE}' ./roco2_conway
#-X -E cpu-cycles -t roco2:metrics -o '/fastfs/tilsche/roco2/lo2s_trace_{DATE}' ./roco2_charon
