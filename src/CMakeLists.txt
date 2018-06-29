add_library(roco2_core STATIC
    asm_kernels.c
    # metrics.cpp
    firestarter_kernel.cpp
    cpu.cpp
    fastcpufreq.c
    c_state_limit.c
)
target_link_libraries(roco2_core PUBLIC firestarter roco2)
add_dependencies(roco2_core libfirestarter)
# add_scorep_instrumentation(roco2_core)

add_library(roco2_main STATIC main.cpp)
target_link_libraries(roco2_main PUBLIC roco2)
# add_scorep_instrumentation(roco2_main)

add_subdirectory(configurations)

add_subdirectory(test)
