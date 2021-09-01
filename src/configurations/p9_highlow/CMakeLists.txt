Roco2Configuration(p9_highlow)
set(P9_HIGHLOW_FREQS "0.5, 2000+-10" CACHE STRING "frequencies to use for experiment")
target_compile_features(roco2_p9_highlow PRIVATE cxx_std_14)
target_compile_definitions(roco2_p9_highlow PRIVATE
    P9_HIGHLOW_FREQS="${P9_HIGHLOW_FREQS}"
    )
configure_file("${CMAKE_CURRENT_LIST_DIR}/run_slurm.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/run_slurm.sh" ESCAPE_QUOTES)

