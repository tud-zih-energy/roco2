#pragma once

#include <roco2/chrono/util.hpp>
#include <roco2/cpu/info.hpp>
#include <roco2/experiments/gpu_sets/gpu_set.hpp>
#include <roco2/metrics/experiment.hpp>
#include <roco2/metrics/metric_guard.hpp>
#include <roco2/metrics/threads.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <atomic>
#include <thread>

#include <cassert>

namespace roco2
{
namespace kernels
{

    class base_gpu_kernel
    {
    public:
        using experiment_tag_t = std::size_t;

        void run(const roco2::experiments::gpu_sets::gpu_set& on)
        {
#pragma omp master 
            {
                assert(!thread_.joinable());

                thread_ = std::thread([&](){     
                    this->run_kernel(on);
                });
            }
        }

        virtual experiment_tag_t tag() const = 0;

        void stop() {
            if (thread_.joinable()) {
                running_ = false;
                thread_.join();
            }
        }

    private:
        virtual void run_kernel(const roco2::experiments::gpu_sets::gpu_set& on) = 0;

        std::thread thread_;

    protected:
        std::atomic<bool> running_ = false;

    public:
        virtual ~base_gpu_kernel()
        {
        }
    };
} // namespace kernels
} // namespace roco2
