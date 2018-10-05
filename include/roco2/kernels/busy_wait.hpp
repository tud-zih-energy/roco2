#ifndef INCLUDE_ROCO2_KERNELS_BUSY_WAIT_HPP
#define INCLUDE_ROCO2_KERNELS_BUSY_WAIT_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{

    class busy_wait : public base_kernel
    {
    public:
        virtual experiment_tag tag() const override
        {
            return 4;
        }

    private:
        void run_kernel(roco2::chrono::time_point tp) override
        {
            SCOREP_USER_REGION("busy_wait_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            std::size_t loops = 0;
            // roco2::chrono::busy_wait_until(tp);
            while (std::chrono::high_resolution_clock::now() < tp)
            {
                loops++;
            }
            roco2::metrics::utility::instance().write(loops);
        }
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_BUSY_WAIT_HPP
