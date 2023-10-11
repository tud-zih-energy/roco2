#include <roco2/initialize.hpp>

#include <roco2/cpu/frequency.hpp>
#include <roco2/cpu/shell.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/addpd.hpp>
#include <roco2/kernels/busy_wait.hpp>
#include <roco2/kernels/compute.hpp>
#include <roco2/kernels/high_low.hpp>
#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/matmul.hpp>
#include <roco2/kernels/memory_copy.hpp>
#include <roco2/kernels/memory_read.hpp>
#include <roco2/kernels/memory_write.hpp>
#include <roco2/kernels/mulpd.hpp>
#include <roco2/kernels/sinus.hpp>
#include <roco2/kernels/sqrt.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <vector>

using namespace roco2::experiments::patterns;

void run_experiments(roco2::chrono::time_point& starting_point, bool eta_only)
{
    roco2::kernels::busy_wait bw;
    roco2::kernels::compute cp;
    roco2::kernels::sinus sinus;
    roco2::kernels::memory_read<> mem_rd;
    roco2::kernels::memory_copy<> mem_cpy;
    roco2::kernels::memory_write<> mem_wrt;
    roco2::kernels::sqrt squareroot;
    roco2::kernels::matmul mm;
    roco2::kernels::idle idle;
    roco2::kernels::mulpd mulpd;
    roco2::kernels::addpd addpd;

    roco2::cpu::frequency freqctl;
    roco2::cpu::shell cstatectl("", "elab cstate enable", "elab cstate enable");

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(3);

    auto freq_list = std::vector<unsigned>{ 2001 };

    auto on_list = block_pattern(1, false, triangle_shape::upper) >>
                   block_pattern(1, false, triangle_shape::lower) >>
                   block_pattern(1, false, triangle_shape::none);
    //    >> stride_pattern(1, 32) >>
    //                   stride_pattern(1, 16) >> stride_pattern(1, 8) >> stride_pattern(1, 4);

    auto cstate_list =
        std::vector<roco2::cpu::shell::setting_type>{ { 2, "elab cstate enable C2" } };

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

#pragma omp master
    {
        roco2::log::info() << "Number of frequencies: " << freq_list.size();
        roco2::log::info() << "Number of placements:  " << on_list.size() << on_list;
    }

#pragma omp barrier

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    auto setting = [&](auto lambda) { plan.push_back(roco2::task::make_lambda_task(lambda)); };

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    for (const auto& cstate_setting : cstate_list)
    {
        setting([&cstatectl, cstate_setting]() { cstatectl.change(cstate_setting); });

        for (const auto& freq : freq_list)
        {
            setting([&freqctl, freq]() { freqctl.change(freq); });

            for (const auto& on : on_list)
            {
                //experiment(mem_cpy, on);
                //experiment(mulpd, on);
                //experiment(squareroot, on);
                experiment(mm, on);
            }
        }

        // do one full idle
        experiment(idle, roco2::experiments::cpu_sets::all_cpus());
    }

    // ------ EDIT TASK PLAN ABOVE THIS LINE ------

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
