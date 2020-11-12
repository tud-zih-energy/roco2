#pragma once

#include <roco2/kernels/asm_kernels.h>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/meta.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{
    class vxor : public base_kernel
    {
    public:
        virtual experiment_tag tag() const override
        {
            return 15;
        }

    private:
        virtual void run_kernel(chrono::time_point until) override
        {
#ifdef HAS_SCOREP
            SCOREP_USER_REGION("vxor_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)
#endif

            std::size_t loops = 0;

            std::uint64_t addr[16];

            auto op1 = roco2::metrics::meta::instance().op1;
            auto op2 = roco2::metrics::meta::instance().op2;

            for (auto i = 0; i < 8; i++)
            {
                addr[i] = op1;
                addr[i + 8] = op2;
            }

            while (std::chrono::high_resolution_clock::now() <= until)
            {
                vxor_kernel(addr, PASSES);

                loops++;
            }

            roco2::metrics::utility::instance().write(loops);
        }

        constexpr static uint64_t PASSES = 10000000;
    };
} // namespace kernels
} // namespace roco2
