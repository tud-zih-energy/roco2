Roco2Configuration(p9_longrun)
target_compile_features(roco2_p9_longrun PRIVATE cxx_std_14)

set(P9_LONGRUN_METRICQ_SERVER "" CACHE STRING "metricq server to preconfigure in SLURM script")
target_compile_definitions(roco2_p9_longrun PRIVATE
    P9_LONGRUN_METRICQ_SERVER="${P9_LONGRUN_METRICQ_SERVER}"
    )

configure_file("${CMAKE_CURRENT_LIST_DIR}/run_slurm.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/run_slurm.sh" ESCAPE_QUOTES)
configure_file("${CMAKE_CURRENT_LIST_DIR}/taurus_metrics_by_node" "${CMAKE_CURRENT_BINARY_DIR}/taurus_metrics_by_node" ESCAPE_QUOTES)
