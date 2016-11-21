
macro(Roco2Configuration CONF )
    add_executable(roco2_${CONF} ${CONF}/main.cpp)

    target_link_libraries(roco2_${CONF} ${LAPACK_LIBRARIES} ${LIBNUMA_LIBRARIES} ${MKL_LIBRARIES} firestarter fastcpufreq ${CPUFREQ_LIBRARIES})

    if(X86Adapt_FOUND)
        target_link_libraries(roco2_${CONF} ${X86ADAPT_LIBRARIES} roco2_core)
    endif()

    add_scorep_instrumentation(roco2_${CONF})

    install(TARGETS roco2_${CONF} RUNTIME DESTINATION bin)
endmacro()
