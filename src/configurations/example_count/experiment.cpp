#include <roco2/initialize.hpp>

#include <roco2/cpu/c_state_limit.hpp>
#include <roco2/cpu/ddcm.hpp>
#include <roco2/cpu/frequency.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/addpd.hpp>
#include <roco2/kernels/busy_wait.hpp>
#include <roco2/kernels/compute.hpp>
#include <roco2/kernels/firestarter.hpp>
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

#include <roco2/experiments/count_return.hpp>
#include <roco2/experiments/timed_return.hpp>

#include <chrono>
#include <string>
#include <vector>

using namespace roco2::experiments::patterns;
using roco2::experiments::count_return;

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only)
{
    roco2::kernels::busy_wait<count_return> bw;
    roco2::kernels::compute<count_return> cp;
    roco2::kernels::sinus<count_return> sinus;
    roco2::kernels::memory_read<count_return> mem_rd;
    roco2::kernels::memory_copy<count_return> mem_cpy;
    roco2::kernels::memory_write<count_return> mem_wrt;
    roco2::kernels::addpd<count_return> addpd;
    roco2::kernels::mulpd<count_return> mulpd;
    roco2::kernels::sqrt<count_return> squareroot;
    roco2::kernels::matmul<count_return> mm;
    roco2::kernels::firestarter<count_return> fs;
    roco2::kernels::idle<count_return> idle(std::chrono::milliseconds(10));

    //    roco2::cpu::frequency freqctl;
    //    roco2::cpu::ddcm ddcm;

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(10);

    // auto freq_list = std::vector<unsigned>{ 2601, 1400 };

    // auto ddcm_list = std::vector<unsigned>{ 1, 5, 10, 15, 16 };

    auto on_list = sub_block_pattern(4, 12) >> block_pattern(4, false, triangle_shape::upper) >>
                   stride_pattern(4, 12);

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

#pragma omp master
    {
        roco2::log::info() << "Experiment list has " << on_list.size() << " entries: " << on_list;
    }

#pragma omp barrier

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);

    auto exp = [experiment_startpoint](auto& cr) {
        return roco2::experiments::const_lenght<count_return>(experiment_startpoint, cr);
    };

    roco2::experiments::count_return cr_bw(353154284000);
    roco2::experiments::count_return cr_cp(103861);
    roco2::experiments::count_return cr_sinus(1131);
    roco2::experiments::count_return cr_mem_rd(632);
    roco2::experiments::count_return cr_mem_cpy(307);
    roco2::experiments::count_return cr_mem_wrt(329);
    roco2::experiments::count_return cr_addpd(4009);
    roco2::experiments::count_return cr_mulpd(7954);
    roco2::experiments::count_return cr_squareroot(2.251);
    roco2::experiments::count_return cr_fs(1605);
    roco2::experiments::count_return cr_mm(47);
    roco2::experiments::count_return cr_idle(1000);

    auto exp_bw = exp(cr_bw);
    auto exp_cp = exp(cr_cp);
    auto exp_sinus = exp(cr_sinus);
    auto exp_mem_rd = exp(cr_mem_rd);
    auto exp_mem_cpy = exp(cr_mem_cpy);
    auto exp_mem_wrt = exp(cr_mem_wrt);
    auto exp_addpd = exp(cr_addpd);
    auto exp_mulpd = exp(cr_mulpd);
    auto exp_squareroot = exp(cr_squareroot);
    auto exp_fs = exp(cr_fs);
    auto exp_mm = exp(cr_mm);
    auto exp_idle = exp(cr_idle);

    // make sure ddcm is disabled
    //    ddcm.disable();

    auto experiment = [&](auto& exp, auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task<count_return>(exp, kernel, on));
    };

    auto setting = [&](auto lambda) { plan.push_back(roco2::task::make_lambda_task(lambda)); };

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    // do one full idle
    // experiment(idle, roco2::experiments::cpu_sets::all_cpus());

    // for each frequency
    //    for (const auto& freq : freq_list)
    //    {
    //        setting([&freqctl, freq]() { freqctl.change(freq); });

    for (const auto& on : on_list)
    {
        //        for (const auto& cycles : ddcm_list)
        //        {
        //            setting([&ddcm, cycles]() { ddcm.change(cycles); });
        experiment(exp_bw, bw, on);
        experiment(exp_cp, cp, on);
        experiment(exp_sinus, sinus, on);
        experiment(exp_mem_rd, mem_rd, on);
        experiment(exp_mem_cpy, mem_cpy, on);
        experiment(exp_mem_wrt, mem_wrt, on);
        experiment(exp_addpd, addpd, on);
        experiment(exp_mulpd, mulpd, on);
        experiment(exp_squareroot, squareroot, on);
        experiment(exp_mm, mm, on);
        experiment(exp_fs, fs, on);
        experiment(exp_idle, idle, on);
        //        }
    }
//    }

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
