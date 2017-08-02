
macro(Roco2Configuration CONF )
    add_executable(roco2_${CONF} experiment.cpp)

    target_link_libraries(
        roco2_${CONF}
        ${LAPACK_LIBRARIES}
        ${LIBNUMA_LIBRARIES}
        ${MKL_LIBRARIES}
        fastcpufreq
        ${FIRESTARTER_LIBRARIES}
        ${CPUFREQ_LIBRARIES}
        roco2_core
        roco2_main
        ${OpenMP_CXX_FLAGS}
    )

    if(X86Adapt_FOUND)
        target_link_libraries(roco2_${CONF} ${X86ADAPT_LIBRARIES})
    endif()

    add_scorep_instrumentation(roco2_${CONF})

    install(TARGETS roco2_${CONF} RUNTIME DESTINATION bin)
endmacro()
