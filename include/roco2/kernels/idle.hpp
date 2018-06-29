#ifndef INCLUDE_ROCO2_KERNELS_IDLE_HPP
#define INCLUDE_ROCO2_KERNELS_IDLE_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/metrics/utility.hpp>
// #include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{

    class idle : public base_kernel
    {
    public:
        virtual experiment_tag tag() const override
        {
            return 2;
        }

    private:
        void run_kernel(roco2::chrono::time_point tp) override
        {
            // SCOREP_USER_REGION("idle_sleep", SCOREP_USER_REGION_TYPE_FUNCTION)

            std::this_thread::sleep_until(tp);
            roco2::metrics::utility::instance().write(1);
        }
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_IDLE_HPP
