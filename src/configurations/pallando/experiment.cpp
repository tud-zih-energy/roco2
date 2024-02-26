#include "roco2/experiments/cpu_sets/cpu_set.hpp"
#include "roco2/experiments/cpu_sets/cpu_set_generator.hpp"
#include <roco2/initialize.hpp>

#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/matmul.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <vector>

using namespace roco2::experiments::patterns;

pattern socket_shuffle() {
    pattern result;

    auto id = 0;

    roco2::experiments::cpu_sets::cpu_set cpus;

    for (int i = 0; i < 104; i++) {
        cpus.add(id);

        if (id + 52 > 103) {
            id = id - 51;
        } else {
            id += 52;
        }

        result.append(cpus);
    }

    return result;
}

pattern random_pattern(int from, int to, int count) {
    pattern result;

    for (int i = from; i <= to; i++) {
        for (int j = 0; j < count; j++) {
            result.append(roco2::experiments::cpu_sets::make_random(i));
        }
    }

    return result;
}

void run_experiments(roco2::chrono::time_point& starting_point, bool eta_only) {
    roco2::kernels::matmul mm;
    roco2::kernels::idle idle;

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(120);

    assert(omp_get_num_threads() == 104);

    auto on_list =
        block_pattern(1, false, triangle_shape::upper) >> //socket_shuffle() >>
        stride_pattern(1, 52) >> (triangle_pattern(0, 12) && triangle_pattern(13, 25) &&
                                  triangle_pattern(26, 38) && triangle_pattern(39, 51)) >>
        stride_pattern(1, 13) >> (triangle_pattern(0, 12) && triangle_pattern(13, 25)) >>
        (triangle_pattern(0, 12) && triangle_pattern(13, 25) && triangle_pattern(27, 39)) >>
        (triangle_pattern(0, 12) && triangle_pattern(13, 25) && triangle_pattern(27, 39) &&
         triangle_pattern(52, 64)) >>
        (triangle_pattern(0, 12) && triangle_pattern(13, 25) && triangle_pattern(52, 64)) >>
        (triangle_pattern(0, 12) && triangle_pattern(13, 25) && triangle_pattern(52, 64) &&
         triangle_pattern(65, 77));
    on_list = on_list >> random_pattern(1, 52, 10);

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

#pragma omp master
    { roco2::log::info() << "Experiment list has " << on_list.size() << " entries: " << on_list; }

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    /* auto setting = [&](auto lambda) { plan.push_back(roco2::task::make_lambda_task(lambda)); };
     */

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    // do one full idle
    experiment(idle, roco2::experiments::cpu_sets::all_cpus());

    for (const auto& on : on_list) {
        experiment(mm, on);
    }

    // ------ EDIT TASK PLAN ABOVE THIS LINE ------

#pragma omp master
    {
        roco2::log::info() << "ETA for whole execution: "
                           << std::chrono::duration_cast<std::chrono::seconds>(plan.eta());
    }

    if (!eta_only) {
#pragma omp barrier

        plan.execute();
    }
}
