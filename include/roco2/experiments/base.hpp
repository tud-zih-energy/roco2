#ifndef INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP

#include <roco2/chrono/chrono.hpp>
#include <roco2/experiments/cpu_sets/cpu_set.hpp>
#include <roco2/experiments/gpu_sets/gpu_set.hpp>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/kernels/base_gpu_kernel.hpp>

#include <chrono>

namespace roco2
{

namespace experiments
{

    class base
    {
    public:
        using time_point = std::chrono::high_resolution_clock::time_point;
        using duration = time_point::duration;

        base(time_point starting_point) : starting_point(starting_point)
        {
        }

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus,
                         roco2::kernels::base_gpu_kernel& gpu_kernel,
                         roco2::experiments::gpu_sets::gpu_set on_gpus) = 0;

        virtual void run(roco2::kernels::base_kernel& cpu_kernel,
                         roco2::experiments::cpu_sets::cpu_set on_cpus) = 0;

        virtual roco2::chrono::duration eta() const = 0;

    protected:
        time_point starting_point;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP
