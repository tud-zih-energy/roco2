
macro(Roco2Configuration CONF)
    add_executable(${CONF}/roco2 ${CONF}/experiment.cpp)

    target_link_libraries(${CONF}/roco2 ${LAPACK_LIBRARIES} ${LIBNUMA_LIBRARIES} ${MKL_LIBRARIES} firestarter fastcpufreq ${CPUFREQ_LIBRARIES} roco2_core)

    if(X86Adapt_FOUND)
        target_link_libraries(${CONF}/roco2 ${X86ADAPT_LIBRARIES})
    endif()

    add_scorep_instrumentation(${CONF}/roco2)

    install(TARGETS ${CONF}/roco2 RUNTIME DESTINATION bin)
endmacro()
