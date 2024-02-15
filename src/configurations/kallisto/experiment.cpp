#include <roco2/initialize.hpp>

#include <roco2/cpu/c_state_limit.hpp>
#include <roco2/cpu/ddcm.hpp>
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
#include <roco2/kernels/sqrt.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <vector>
#include <cassert>

using namespace roco2::experiments::patterns;

enum class core_type {
    non_ht,
    ht,
    ecore,
};

/**
 * returns number of linux CPU for number of core type
 * e.g.
 *
 * | non_ht | 0 | 0  |
 * | non_ht | 1 | 2  |
 * | ht     | 1 | 3  |
 * | ht     | 2 | 5  |
 * | ecore  | 0 | 16 |
 * | ecore  | 1 | 17 |
 */
static int get_linux_cpu_num(core_type cpu_type, int cpu_num) {
    assert(cpu_num >= 0 && cpu_num <= 7);

    switch(cpu_type) {
    case core_type::non_ht:
        return cpu_num * 2;
    case core_type::ht:
        return 1 + cpu_num * 2;
    case core_type::ecore:
        return 16 + cpu_num;
    default:
        throw std::runtime_error("unkown CPU type");
    };
}

/// retrieve CPU patterns for alderlake
static pattern get_alderlake_patterns() {
    // this function generates the patterns for alderlake
    // due to the mapping of CPU num -> their functionality the traditional roco2 pattern generators are not applicable

    // the CPUs are mapped as follows:
    //  0-15 P-cores
    // 16-23 E-cores
    // for P-cores where **even** numbers are the cores, and the following **odd** number is the corresponding HT cpu
    //
    // $ lscpu --all --extended
    // CPU NODE SOCKET CORE L1d:L1i:L2:L3 ONLINE    MAXMHZ   MINMHZ      MHZ
    //   0    0      0    0 0:0:0:0          yes 3201.0000 800.0000  800.000
    //   1    0      0    0 0:0:0:0          yes 3201.0000 800.0000 3201.000
    //   2    0      0    1 4:4:1:0          yes 3201.0000 800.0000  800.000
    //   3    0      0    1 4:4:1:0          yes 3201.0000 800.0000 3201.000
    //   4    0      0    2 8:8:2:0          yes 3201.0000 800.0000  800.000
    // ...
    //
    // this is handled by the method get_linux_cpu_num() above

    pattern all_patterns;

    // Part A: non-HT, HT, E-cores separate
    for (core_type cpu_type : {core_type::non_ht, core_type::ht, core_type::ecore}) {
        std::vector<bool> on_list(24);
        // build triangle
        for (int max_enabled_cpu = 0; max_enabled_cpu < 8; max_enabled_cpu++) {
            // note: CPUs from previous iteration are still enabled, no need to re-build triangle every iteration
            on_list[get_linux_cpu_num(cpu_type, max_enabled_cpu)] = true;

            all_patterns = all_patterns >> raw_pattern(on_list);
        }
    }

    // Part B: enabling non-HT, HT, E-cores subsequently (in that order)
    {
        std::vector<bool> on_list(24);
        for (core_type cpu_type : {core_type::non_ht, core_type::ht, core_type::ecore}) {
            for (int max_enabled_cpu = 0; max_enabled_cpu < 8; max_enabled_cpu++) {
                on_list[get_linux_cpu_num(cpu_type, max_enabled_cpu)] = true;
                all_patterns = all_patterns >> raw_pattern(on_list);
            }
        }
    }

    // Part C: enable first non-HT, HT, E-core, then second non-HT, HT, E-core...
    {
        std::vector<bool> on_list(24);
        for (int max_enabled_cpu = 0; max_enabled_cpu < 8; max_enabled_cpu++) {
            on_list[get_linux_cpu_num(core_type::ht, max_enabled_cpu)] = true;
            on_list[get_linux_cpu_num(core_type::non_ht, max_enabled_cpu)] = true;
            on_list[get_linux_cpu_num(core_type::ecore, max_enabled_cpu)] = true;

            all_patterns = all_patterns >> raw_pattern(on_list);
        }
    }

    return all_patterns;
}

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only)
{
    roco2::kernels::busy_wait bw;
    roco2::kernels::compute cp;
    roco2::kernels::memory_read<> mem_rd;
    roco2::kernels::memory_copy<> mem_cpy;
    roco2::kernels::memory_write<> mem_wrt;
    roco2::kernels::sqrt squareroot;
    roco2::kernels::matmul mm;
    roco2::kernels::idle idle;
    roco2::kernels::mulpd mulpd;
    roco2::kernels::addpd addpd;

    roco2::cpu::frequency freqctl;

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(10);

    // copied from: /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
    // note that both P- and E-cores report **the same** available frequencies
    auto freq_list = std::vector<unsigned>{ 3201, 3200, 2500, 1700, 800 };

    auto on_list = get_alderlake_patterns();

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

#pragma omp master
    {
        roco2::log::info() << "Experiment list has " << on_list.size() << " entries: " << on_list;
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

    setting([&freqctl, &freq_list]() { freqctl.change(freq_list[0]); });

    // for each frequency
    for (const auto& freq : freq_list)
    {
        setting([&freqctl, freq]() { freqctl.change(freq); });

        // for reference
        experiment(idle, roco2::experiments::cpu_sets::all_cpus());

        for (const auto& on : on_list)
        {
            experiment(bw, on);
            experiment(cp, on);
            experiment(mem_rd, on);
            experiment(mem_cpy, on);
            experiment(mem_wrt, on);
            experiment(addpd, on);
            experiment(mulpd, on);
            experiment(squareroot, on);
            experiment(mm, on);
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
