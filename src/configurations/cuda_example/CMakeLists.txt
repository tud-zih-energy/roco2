if (X86Adapt_FOUND AND CpuFreq_FOUND AND USE_ASM_KERNELS AND USE_CUDA)
    Roco2Configuration(cuda_example)
endif()
