
macro(Roco2Configuration CONF )
    add_executable(roco2_${CONF} ${CONF}/main.cpp)

    target_link_libraries(roco2_${CONF} ${LAPACK_LIBRARIES} ${LIBNUMA_LIBRARIES} ${MKL_LIBRARIES} firestarter fastcpufreq ${CPUFREQ_LIBRARIES} roco2_core)

    set_target_properties(roco2_${CONF} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CONF}")

    if(X86Adapt_FOUND)
        target_link_libraries(roco2_${CONF} ${X86ADAPT_LIBRARIES})
    endif()

    add_scorep_instrumentation(roco2_${CONF})
    set_target_properties(roco2_${CONF}_scorep PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CONF}")


    install(TARGETS roco2_${CONF} RUNTIME DESTINATION bin)
endmacro()
