#pragma once

#include <roco2/chrono/util.hpp>
#include <roco2/experiments/gpu_sets/gpu_set.hpp>
#include <roco2/metrics/experiment.hpp>
#include <roco2/metrics/metric_guard.hpp>
#include <roco2/metrics/threads.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <atomic>
#include <thread>
#include <vector>

#include <cassert>

namespace roco2
{
namespace kernels
{

    class base_gpu_kernel
    {
    public:
        using experiment_tag_t = std::size_t;

        void base_gpu_kernel::run(const roco2::experiments::gpu_sets::gpu_set& on)
        {
#pragma omp master
            {
                assert(threads_.empty());

                for (int gpu_id = 0; gpu_id < on.max(); gpu_id++)
                {
                    if (!on.contains(gpu_id))
                    {
                        continue;
                    }

                    threads_.emplace_back([this, gpu_id]() { this->run_kernel(gpu_id); });
                }
            }
        }

        virtual experiment_tag_t tag() const = 0;

        void stop()
        {
            running_ = false;

            for (auto& thread : threads_)
            {
                assert(thread.joinable());
                thread.join();
            }

            threads_.clear();
        }

    private:
        virtual void run_kernel(int on_gpu) = 0;

        std::vector<std::thread> threads_;

    protected:
        std::atomic<bool> running_ = false;

    public:
        virtual ~base_gpu_kernel()
        {
        }
    };
} // namespace kernels
} // namespace roco2
