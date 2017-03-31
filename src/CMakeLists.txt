include_directories(${FIRESTARTER_INCLUDE_DIRS})

add_library(fastcpufreq fastcpufreq.c)
add_library(c_state_limit c_state_limit.c)

add_library(roco2_core STATIC asm_kernels.c metrics.cpp firestarter_kernel.cpp)
target_link_libraries(roco2_core ${FIRESTARTER_LIBRARIES})
add_scorep_instrumentation(roco2_core)

add_subdirectory(configurations)

add_subdirectory(test)
