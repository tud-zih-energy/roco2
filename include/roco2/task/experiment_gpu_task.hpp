#pragma once

#include <roco2/task/task.hpp>

#include <roco2/experiments/base.hpp>
#include <roco2/kernels/base_gpu_kernel.hpp>
#include <roco2/kernels/base_kernel.hpp>

namespace roco2
{
namespace task
{
    class experiment_gpu_task : public task
    {
    public:
        experiment_gpu_task(roco2::experiments::base& exp, roco2::kernels::base_kernel& kernel,
                            const roco2::experiments::cpu_sets::cpu_set& on,
                            roco2::kernels::base_gpu_kernel& gpu_kernel,
                            const roco2::experiments::gpu_sets::gpu_set& on_gpu)
        : exp_(exp), kernel_(kernel), on_(on), gpu_kernel_(gpu_kernel), on_gpu_(on_gpu)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return exp_.eta();
        }

        virtual void execute() override
        {
            exp_.run(kernel_, on_, gpu_kernel_, on_gpu_);
        }

    private:
        roco2::experiments::base& exp_;
        roco2::kernels::base_kernel& kernel_;
        roco2::experiments::cpu_sets::cpu_set on_;
        roco2::kernels::base_gpu_kernel& gpu_kernel_;
        roco2::experiments::gpu_sets::gpu_set on_gpu_;
    };
} // namespace task
} // namespace roco2
