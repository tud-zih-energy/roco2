Roco2Configuration(highlow)
set(P9_HIGHLOW_FREQS "0.5, 2000+-10" CACHE STRING "frequencies to use for experiment")
target_compile_features(roco2_highlow PRIVATE cxx_std_14)
target_compile_definitions(roco2_highlow PRIVATE
    P9_HIGHLOW_FREQS="${P9_HIGHLOW_FREQS}"
    )
