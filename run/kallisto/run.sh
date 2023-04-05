#!/bin/bash

source /etc/profile.d/lmod.sh
source /etc/profile.d/zih-a-lmod.sh

module purge --force
module load toolchain/system scorep_metricq lo2s elab

echo "it is $(date)"

export GOMP_CPU_AFFINITY=0-23
export OPENBLAS_NUM_THREADS=1

export SCOREP_ENABLE_TRACING=1
export SCOREP_ENABLE_PROFILING=0
export SCOREP_TOTAL_MEMORY=4095M
export SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT=48h

echo "environment variables:"
echo "  GOMP_CPU_AFFINITY                    = $GOMP_CPU_AFFINITY"
echo "  SCOREP_ENABLE_TRACING                = $SCOREP_ENABLE_TRACING"
echo "  SCOREP_ENABLE_PROFILING              = $SCOREP_ENABLE_PROFILING"
echo "  SCOREP_TOTAL_MEMORY                  = $SCOREP_TOTAL_MEMORY"
echo "  SCOREP_METRIC_PLUGINS                = $SCOREP_METRIC_PLUGINS"
echo "  SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT = $SCOREP_METRIC_METRICQ_PLUGIN_TIMEOUT"

echo "executing test..."

ulimit -n 999999
elab frequency turbo

sudo perf probe -d roco2:metrics

ROCO2=../../build_kallisto/src/configurations/kallisto/roco2_kallisto
LO2S=$HOME/lo2s/BUILD/lo2s

sudo perf probe -x $ROCO2 roco2:metrics=_ZN5roco27metrics4meta5writeEmmlmmmm experiment frequency shell threads utility op1 op2 || exit 1


# getting probe permissions sucks
sudo -E env "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" $LO2S \
-A -X -S -t roco2:metrics -i 100 \
-e cpu-clock -c 32000000 \
--userspace-metric-event uncore_clock/clockticks/ \
--userspace-metric-event r228 --userspace-metric-event r428 --userspace-metric-event r828 --userspace-metric-event r128 \
-E msr/aperf/ -E instructions --metric-frequency 10 \
-- $ROCO2
sudo chown -R $USER lo2s_trace_*
echo "done"
