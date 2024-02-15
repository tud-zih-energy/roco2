if(CpuFreq_FOUND AND USE_ASM_KERNELS)
    Roco2Configuration(kallisto)

    configure_file("${CMAKE_CURRENT_LIST_DIR}/run.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/run.sh" ESCAPE_QUOTES)
endif()


