#include <roco2/initialize.hpp>

#include <roco2/cpu/c_state_limit.hpp>
#include <roco2/cpu/frequency.hpp>
#include <roco2/cpu/shell.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/vxor.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <vector>

using namespace roco2::experiments::patterns;

std::uint64_t operand(int hw)
{
    auto op = ~0ull;

    return op >> (64 - hw);
}

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only)
{
    roco2::kernels::vxor vxor;
    roco2::kernels::idle idle;

    roco2::cpu::frequency freqctl;
    roco2::cpu::shell cstatectl("", "elab cstate enable", "elab cstate enable");

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::milliseconds(10000);

    auto freq_list = std::vector<unsigned>{ 2500, 2200, 1500 };

    auto operands = { operand(0),  operand(8),  operand(16), operand(24), operand(32),
                      operand(40), operand(48), operand(56), operand(64) };

    auto cstate_list =
        std::vector<roco2::cpu::shell::setting_type>{ { 0, "elab cstate enable --only POLL" },
                                                      { 1, "elab cstate enable C1" },
                                                      { 2, "elab cstate enable C2" } };

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

    // #pragma omp master
    //     {
    //         roco2::log::info() << "Experiment list has " << on_list.size() << " entries: " <<
    //         on_list;
    //     }

#pragma omp barrier

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    auto setting = [&](auto lambda) { plan.push_back(roco2::task::make_lambda_task(lambda)); };

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    setting([&freqctl, &freq_list]() { freqctl.change(freq_list[0]); });

    for (const auto& cstate_setting : cstate_list)
    {
        setting([&cstatectl, cstate_setting]() { cstatectl.change(cstate_setting); });

        // do one full idle
        experiment(idle, roco2::experiments::cpu_sets::all_cpus());

        // for each frequency
        for (const auto& freq : freq_list)
        {
            setting([&freqctl, freq]() { freqctl.change(freq); });

            for (const auto& op1 : operands)
            {
                setting([op1]() { roco2::metrics::meta::instance().op1 = op1; });

                for (const auto& op2 : operands)
                {
                    setting([op2]() { roco2::metrics::meta::instance().op2 = op2; });

                    experiment(vxor, roco2::experiments::cpu_sets::all_cpus());
                }
            }
        }
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
