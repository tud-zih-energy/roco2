set(roco2_core_SOURCES)
list(APPEND roco2_core_SOURCES
    metrics.cpp
    cpu.cpp
    c_state_limit.c
    )

if (CpuFreq_FOUND) 
    message(STATUS "Building WITH cpufreq.")
    list(APPEND roco2_core_SOURCES fastcpufreq.c)
else()
    message(STATUS "Building WITHOUT cpufreq.")
endif()

if (USE_ASM_KERNELS)
    message(STATUS "Building WITH asm kernels.")
    list(APPEND roco2_core_SOURCES asm_kernels.c)
else()
    message(STATUS "Building WITHOUT asm kernels.")
endif()

if (USE_FIRESTARTER)
    list(APPEND roco2_core_SOURCES firestarter_kernel.cpp)
endif()

add_library(roco2_core STATIC ${roco2_core_SOURCES})
target_link_libraries(roco2_core PUBLIC roco2)
if (USE_FIRESTARTER)
    target_compile_definitions(roco2_core PUBLIC "HAS_FIRESTARTER")
    target_link_libraries(roco2_core PUBLIC firestarter)
    add_dependencies(roco2_core libfirestarter)
endif()
if (USE_SCOREP)
    option(ROCO2_HIGHLOW_INSTRUMENT_PHASES "instrument the individual high & low phases of the high-low-kernel" OFF)
    if (ROCO2_HIGHLOW_INSTRUMENT_PHASES)
        target_compile_definitions(roco2_core PUBLIC "ROCO2_HIGHLOW_INSTRUMENT_PHASES")
    endif()
endif()

add_library(roco2_main STATIC main.cpp)
target_link_libraries(roco2_main PUBLIC roco2)

add_subdirectory(configurations)

add_subdirectory(test)
