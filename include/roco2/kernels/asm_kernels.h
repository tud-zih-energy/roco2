#ifndef INCLUDE_ROCO2_KERNELS_ASM_KERNELS_H
#define INCLUDE_ROCO2_KERNELS_ASM_KERNELS_H

#ifdef __cplusplus
#define EXTERN_C extern "C"
#include <cstdint>
#else
#define EXTERN_C
#include <stdint.h>
#endif

EXTERN_C uint64_t mulpd_kernel(double* buffer, uint64_t repeat);
EXTERN_C uint64_t addpd_kernel(double* buffer, uint64_t repeat);

EXTERN_C uint64_t sqrtss_kernel(float* buffer, uint64_t elems, uint64_t repeat);
EXTERN_C uint64_t sqrtsd_kernel(double* buffer, uint64_t elems, uint64_t repeat);
EXTERN_C uint64_t sqrtps_kernel(float* buffer, uint64_t elems, uint64_t repeat);
EXTERN_C uint64_t sqrtpd_kernel(double* buffer, uint64_t elems, uint64_t repeat);
EXTERN_C void vxor_kernel(uint64_t addr[16], uint64_t passes);

#endif // INCLUDE_ROCO2_KERNELS_ASM_KERNELS_H
