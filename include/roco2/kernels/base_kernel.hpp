#ifndef INCLUDE_ROCO2_KERNELS_KERNEL_HPP
#define INCLUDE_ROCO2_KERNELS_KERNEL_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/cpu/info.hpp>
#include <roco2/experiments/cpu_sets/cpu_set.hpp>
#include <roco2/metrics/experiment.hpp>
#include <roco2/metrics/metric_guard.hpp>
#include <roco2/metrics/threads.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace roco2
{
namespace kernels
{

    class base_kernel
    {
    public:
        using experiment_tag = std::size_t;

        void run(roco2::chrono::time_point until, roco2::experiments::cpu_sets::cpu_set on)
        {
            roco2::metrics::threads::instance().write(on.num_threads());
            roco2::metrics::metric_guard<roco2::metrics::experiment> guard(this->tag());
            if (std::find(on.begin(), on.end(), roco2::cpu::info::current_thread()) != on.end())
            {

                this->run_kernel(until);
            }
            else
            {
                SCOREP_USER_REGION("idle_sleep", SCOREP_USER_REGION_TYPE_FUNCTION)

                std::this_thread::sleep_until(until);

                roco2::metrics::utility::instance().write(1);
            }
        }

        virtual experiment_tag tag() const = 0;

    private:
        virtual void run_kernel(roco2::chrono::time_point until) = 0;

    public:
        virtual ~base_kernel()
        {
        }
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_KERNEL_HPP
