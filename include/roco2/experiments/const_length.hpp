#ifndef INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP

#include <roco2/experiments/base.hpp>

namespace roco2
{

namespace experiments
{

    class const_lenght : public base
    {
    public:
        using base::duration;
        using base::time_point;

        const_lenght(time_point starting_point, duration length)
        : base(starting_point), length(length)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return length;
        }

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus,
                         roco2::kernels::base_gpu_kernel& gpu_kernel,
                         roco2::experiments::gpu_sets::gpu_set on_gpus) override
        {
            starting_point += length;

            roco2::metrics::threads::instance().write(on_cpus.num_threads());
            roco2::metrics::metric_guard<roco2::metrics::experiment> guard(cpu_kernel.tag() +
                                                                           gpu_kernel.tag());

            gpu_kernel.setup_streams(on_gpus);
            cpu_kernel.run(starting_point, on_cpus, [&gpu_kernel]() { gpu_kernel.progress(); });
            gpu_kernel.stop();
        }

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus) override
        {
            starting_point += length;

            roco2::metrics::threads::instance().write(on_cpus.num_threads());
            roco2::metrics::metric_guard<roco2::metrics::experiment> guard(cpu_kernel.tag());

            cpu_kernel.run(starting_point, on_cpus, [](){});
        }

    private:
        duration length;
    };
} // namespace experiments
} // namespace roco2

#endif // INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
