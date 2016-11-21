#ifndef INCLUDE_ROCO2_KERNELS_ADDPD_HPP
#define INCLUDE_ROCO2_KERNELS_ADDPD_HPP

#include <roco2/kernels/asm_kernels.h>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <limits>

namespace roco2
{
namespace kernels
{
    class addpd : public base_kernel
    {
    private:
        virtual experiment_tag tag() const override
        {
            return 3;
        }

        virtual void run_kernel(chrono::time_point until) override
        {
            SCOREP_USER_REGION("addpd_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            auto& comp_A = thread_local_memory().vec_A;

            for (std::size_t i = 0; i < 16; ++i)
            {
                comp_A[i] = 1. + std::numeric_limits<double>::epsilon();
            }

            std::size_t loops = 0;

            do
            {
                // SCOREP_USER_REGION("addpd_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)

                addpd_kernel(comp_A.data(), repeat);
                loops++;

            } while (std::chrono::high_resolution_clock::now() < until);

            roco2::metrics::utility::instance().write(loops);
        }

        // should be around 2ms per call of mulpd_kernel
        static const std::size_t repeat = 4096 * 1024;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_ADDPD_HPP
