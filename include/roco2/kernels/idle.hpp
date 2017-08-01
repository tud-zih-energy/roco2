#ifndef INCLUDE_ROCO2_KERNELS_IDLE_HPP
#define INCLUDE_ROCO2_KERNELS_IDLE_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{
    template <typename return_functor>
    class idle : public base_kernel<return_functor>
    {
    public:
        idle(roco2::chrono::duration iteration_duration)
        : base_kernel<return_functor>(), iteration_duration(iteration_duration)
        {
        }

        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 2;
        }

    private:
        void run_kernel(return_functor& cond) override
        {
            SCOREP_USER_REGION("idle_sleep", SCOREP_USER_REGION_TYPE_FUNCTION)
            int iteration = 0;
            while (cond())
            {
                std::this_thread::sleep_until(roco2::chrono::now() + iteration_duration);
                iteration++;
            }
            roco2::metrics::utility::instance().write(iteration);
        }

        roco2::chrono::duration iteration_duration;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_IDLE_HPP
