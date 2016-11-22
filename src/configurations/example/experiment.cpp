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
// #include <roco2/kernels/firestarter.hpp>
#include <roco2/kernels/high_low.hpp>
#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/matmul.hpp>
#include <roco2/kernels/memory.hpp>
#include <roco2/kernels/mulpd.hpp>
#include <roco2/kernels/sinus.hpp>
#include <roco2/kernels/sqrt.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <nitro/broken_options/parser.hpp>

#include <string>
#include <vector>

#include <omp.h>

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only);

int main(int argc, char** argv)
{
    auto starting_point = roco2::chrono::now();

    roco2::log::info() << "Starting initialize at: " << starting_point.time_since_epoch().count();

    // first assumption:
    // GOMP_CPU_AFFINITY or respectivly KMP_AFFINITY was set to a sane value.
    nitro::broken_options::parser parser;

    parser.toggle("debug").short_name("d");
    parser.toggle("eta_only").short_name("e");

    auto options = parser.parse(argc, argv);

    if (options.given("debug"))
    {
        roco2::logging::filter<roco2::logging::record>::set_severity(
            nitro::log::severity_level::debug);
    }
    else
    {
        roco2::logging::filter<roco2::logging::record>::set_severity(
            nitro::log::severity_level::info);
    }

    bool eta_only = options.given("eta_only");

    // TODO: What is this for?
    omp_set_dynamic(0);

    roco2::log::info() << "Starting with " << omp_get_max_threads() << " threads.";

    roco2::initialize::master();

    // Throwing exceptions out of a parellel regions doesn't seem smart.
    // Therefore we have this boolean flag here.
    bool exception_happend = false;

#pragma omp parallel default(shared) shared(starting_point,                                        \
                                            exception_happend) firstprivate(eta_only)
    {
        try
        {
            run_experiments(starting_point, eta_only);
        }
        catch (std::exception& e)
        {
            roco2::log::fatal() << "An exception happend during execution: " << e.what();
            exception_happend = true;
        }
    }

    if (!exception_happend)
    {
        roco2::log::info() << "Successfully set your machine on fire.";

        return 0;
    }
    else
    {
        return 1;
    }
}

using namespace roco2::experiments::patterns;

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only)
{
    roco2::kernels::busy_wait bw;
    roco2::kernels::compute cp;
    roco2::kernels::sinus sinus;
    roco2::kernels::memory mem;
    roco2::kernels::sqrt squareroot;
    roco2::kernels::matmul mm;
    // roco2::kernels::firestarter fs;
    roco2::kernels::idle idle;
    roco2::kernels::mulpd mulpd;
    roco2::kernels::addpd addpd;

    roco2::cpu::frequency freqctl;
    roco2::cpu::ddcm ddcm;

    roco2::memory::numa_bind_local nbl;

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(10);

    auto freq_list = std::vector<unsigned>{ 2601, 1400 };

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

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    // make sure ddcm is disabled
    ddcm.disable();

    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    auto setting = [&](auto lambda) { plan.push_back(roco2::task::make_lambda_task(lambda)); };

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    // do one full idle
    experiment(idle, roco2::experiments::cpu_sets::all_cpus());

    // for each frequency
    for (const auto& freq : freq_list)
    {
        setting([&freqctl, freq]() { freqctl.change(freq); });

        for (const auto& on : on_list)
        {

            for (auto cycles : { 1, 5, 10, 15, 16 })
            {
                setting([&ddcm, cycles]() { ddcm.change(cycles); });

                experiment(bw, on);
                experiment(cp, on);
                experiment(sinus, on);
                experiment(mem, on);
                experiment(addpd, on);
                experiment(mulpd, on);
                experiment(squareroot, on);
                experiment(mm, on);
                // experiment(fs, on);
            }
        }
    }

    // ------ EDIT TASK PLAN ABOVE THIS LINE ------

    if (eta_only)
    {
#pragma omp master
        {
            roco2::log::info() << "ETA for whole execution: "
                               << std::chrono::duration_cast<std::chrono::seconds>(plan.eta());
        }
    }
    else
    {
#pragma omp barrier

        plan.execute();
    }
}
