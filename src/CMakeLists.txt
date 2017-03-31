# add_subdirectory(firestarter)

include_directories(firestarter)

add_library(fastcpufreq fastcpufreq.c)
add_library(c_state_limit c_state_limit.c)

add_library(roco2_core STATIC asm_kernels.c metrics.cpp firestarter_kernel.cpp)
add_scorep_instrumentation(roco2_core)

add_subdirectory(configurations)

add_subdirectory(test)
