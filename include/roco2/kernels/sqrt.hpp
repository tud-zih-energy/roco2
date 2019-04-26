#ifndef INCLUDE_ROCO2_KERNELS_SQRT_HPP
#define INCLUDE_ROCO2_KERNELS_SQRT_HPP

#include <roco2/kernels/asm_kernels.h>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
// #include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{
    class sqrt : public base_kernel
    {
    private:
        virtual experiment_tag tag() const override
        {
            return 11;
        }

        virtual void run_kernel(chrono::time_point until) override
        {
#ifdef HAS_SCOREP
            SCOREP_USER_REGION("sqrt_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)
#endif

            auto& comp_A = thread_local_memory().vec_A;
            auto& comp_F = thread_local_memory().vec_F;

            std::size_t loops = 0;

            do
            {
                // SCOREP_USER_REGION("sqrt_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)
                switch (type)
                {
                case 0:
                    sqrtss_kernel(comp_F.data(), comp_F.size(), repeat);
                    break;
                case 1:
                    sqrtps_kernel(comp_F.data(), comp_F.size(), repeat);
                    break;
                case 2:
                    sqrtsd_kernel(comp_A.data(), comp_A.size(), repeat);
                    break;
                case 3:
                    sqrtpd_kernel(comp_A.data(), comp_A.size(), repeat);
                    break;
                }

                loops++;
            } while (std::chrono::high_resolution_clock::now() < until);

            roco2::metrics::utility::instance().write(loops);
        }

        static const std::size_t repeat = 1024; // 4096;
        static const std::size_t type = 2;
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_SQRT_HPP
