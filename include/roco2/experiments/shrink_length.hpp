#ifndef INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP

#include <roco2/experiments/base.hpp>
#include <roco2/log.hpp>

#include <algorithm>

namespace roco2
{

namespace experiments
{

    class shrink_lenght : public base
    {
    public:
        using base::time_point;
        using base::duration;

        shrink_lenght(time_point starting_point, duration starting_length, duration min_length,
                      double factor)
        : base(starting_point), length(starting_length), min_length(min_length), factor(factor)
        {
            log::warn() << "Using shrink_lenght experiments. ETA might be not correct at all.";
        }

        virtual roco2::chrono::duration eta() const override
        {
            if (length > min_length)
            {
                return std::max(length * factor, min_length);
            }
            else
            {
                return length;
            }
        }

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus,
                         roco2::kernels::base_gpu_kernel& gpu_kernel,
                         roco2::experiments::gpu_sets::gpu_set on_gpus) override
        {
            if (length > min_length)
            {
                length = std::max(length * factor, min_length);
            }

            roco2::metrics::threads::instance().write(on_cpus.num_threads());
            roco2::metrics::metric_guard<roco2::metrics::experiment> guard(cpu_kernel.tag() + gpu_kernel.tag());

            starting_point += length;
            gpu_kernel.run(on_gpus);
            cpu_kernel.run(starting_point, on_cpus);
            gpu_kernel.stop();
        }

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus) override
        {
            if (length > min_length)
            {
                length = std::max(length * factor, min_length);
            }

            roco2::metrics::threads::instance().write(on_cpus.num_threads());
            roco2::metrics::metric_guard<roco2::metrics::experiment> guard(cpu_kernel.tag());

            starting_point += length;
            cpu_kernel.run(starting_point, on_cpus);
        }

    private:
        duration length;
        duration min_length;
        double factor;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP
