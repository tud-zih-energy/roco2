#ifndef INCLUDE_ROCO2_KERNELS_MATMUL_HPP
#define INCLUDE_ROCO2_KERNELS_MATMUL_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>

#ifdef USE_MKL
#include <mkl_cblas.h>
#endif

#ifdef USE_CBLAS
#include <cblas.h>
#endif

#ifdef USE_ACML
#include <acml.h>
#endif

#include <chrono>

namespace roco2
{
namespace kernels
{
    class matmul : public base_kernel
    {
        virtual experiment_tag tag() const override
        {
            return 7;
        }

        virtual void run_kernel(chrono::time_point until) override
        {
            SCOREP_USER_REGION("matmul_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            double* A = roco2::thread_local_memory().mat_A.data();
            double* B = roco2::thread_local_memory().mat_B.data();
            double* C = roco2::thread_local_memory().mat_C.data();

            uint64_t m = roco2::thread_local_memory().mat_size;

            std::size_t loops = 0;

            do
            {
                // SCOREP_USER_REGION("matmul_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)

#if defined USE_MKL || defined USE_CBLAS
                cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, m, m, 1.0, A, m, B, m,
                            1.0, C, m);
#else
                // ACML
                dgemm('N', 'N', m, m, m, 1.0, A, m, B, m, 1.0, C, m);
#endif

                loops++;
            } while (std::chrono::high_resolution_clock::now() < until);

            roco2::metrics::utility::instance().write(loops);
        }
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_MATMUL_HPP
