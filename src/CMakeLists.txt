set(roco2_core_SOURCES)
list(APPEND roco2_core_SOURCES
    main.cpp
    metrics.cpp
    cpu.cpp
    fastcpufreq.c
    c_state_limit.c
    )

if (USE_ASM_KERNELS)
    message(STATUS "Building WITH asm kernels.")
    list(APPEND roco2_core_SOURCES asm_kernels.c)
else()
    message(STATUS "Building WITHOUT asm kernels.")
endif()

list(APPEND roco2_core_SOURCES firestarter_kernel.cpp)

add_library(roco2_core STATIC ${roco2_core_SOURCES})

target_link_libraries(roco2_core PUBLIC roco2 Nitro::env Nitro::options)

target_link_libraries(roco2_core PUBLIC
    firestartercore
    )

if (USE_SCOREP)
    option(ROCO2_HIGHLOW_INSTRUMENT_PHASES "instrument the individual high & low phases of the high-low-kernel" OFF)
    if (ROCO2_HIGHLOW_INSTRUMENT_PHASES)
        target_compile_definitions(roco2_core PUBLIC "ROCO2_HIGHLOW_INSTRUMENT_PHASES")
    endif()
endif()

add_subdirectory(configurations)

add_subdirectory(test)
