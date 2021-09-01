#include <roco2/initialize.hpp>

#include <roco2/cpu/shell.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/kernels/busy_wait.hpp>
#include <roco2/kernels/compute.hpp>
#include <roco2/kernels/high_low.hpp>
#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/memory_copy.hpp>
#include <roco2/kernels/memory_read.hpp>
#include <roco2/kernels/memory_write.hpp>
#include <roco2/kernels/sinus.hpp>
#include <roco2/kernels/matmul.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <vector>
#include <memory>

using namespace roco2::experiments::patterns;
namespace kernels = roco2::kernels;

// runs all suitable kernels
void run_experiments(roco2::chrono::time_point starting_point, bool eta_only)
{
    std::vector<std::shared_ptr<kernels::base_kernel>> kernel_list = {
        std::make_shared<kernels::busy_wait>(),
        std::make_shared<kernels::memory_copy<>>(),
        std::make_shared<kernels::compute>(),
        std::make_shared<kernels::memory_write<>>(),
        std::make_shared<kernels::sinus>(),
        std::make_shared<kernels::matmul>(),
        std::make_shared<kernels::memory_read<>>(),
    };
    auto experiment_duration = std::chrono::seconds(60);
    auto on_list = block_pattern(4, false, triangle_shape::upper);
    roco2::task::task_plan plan;

#pragma omp master
    {
        roco2::log::info() << "Number of placements:  " << on_list.size() << on_list;
    }
#pragma omp barrier

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);
    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);
    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    // actual task plan
    for (auto& k : kernel_list) {
        for (const auto& on : on_list) {
            experiment(*k, on);
        }
    }

#pragma omp master
    {
        roco2::log::info() << "ETA for whole execution: "
                           << std::chrono::duration_cast<std::chrono::seconds>(plan.eta());
    }

    if (!eta_only)
    {
#pragma omp barrier 
        plan.execute();
    }
}
