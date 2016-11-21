#include <roco2/initialize.hpp>

#include <roco2/log.hpp>

#include <roco2/cpu/ddcm.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/addpd.hpp>
#include <roco2/kernels/busy_wait.hpp>
#include <roco2/kernels/compute.hpp>
#include <roco2/kernels/firestarter.hpp>
#include <roco2/kernels/high_low.hpp>
#include <roco2/kernels/idle.hpp>
#include <roco2/kernels/matmul.hpp>
#include <roco2/kernels/memory.hpp>
#include <roco2/kernels/mulpd.hpp>
#include <roco2/kernels/sinus.hpp>
#include <roco2/kernels/sqrt.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/cpu/frequency.hpp>

#include <string>
#include <vector>

#include <omp.h>

template <typename TimePoint>
void run_experiments(TimePoint& starting_point)
{
    roco2::kernels::busy_wait bw;
    roco2::kernels::high_low_bs hl1(std::chrono::milliseconds(25), std::chrono::milliseconds(25));
    roco2::kernels::high_low_bs hl2(std::chrono::milliseconds(50), std::chrono::milliseconds(50));
    roco2::kernels::high_low_bs hl3(std::chrono::milliseconds(100), std::chrono::milliseconds(100));
    roco2::kernels::compute cp;
    roco2::kernels::sinus sinus;
    roco2::kernels::memory mem;
    roco2::kernels::sqrt squareroot;
    roco2::kernels::matmul mm;
    roco2::kernels::firestarter fs;
    roco2::kernels::idle idle;
    roco2::kernels::mulpd mulpd;
    roco2::kernels::addpd addpd;

    roco2::cpu::frequency freqctl;
    roco2::cpu::ddcm ddcm;

    roco2::memory::numa_bind_local nbl;

    using namespace roco2::experiments::patterns;

    auto experiment_duration = std::chrono::seconds(10);

    auto freq_list = std::vector<unsigned>{ 2601, 1400 };

    auto on_list = // sub_block_pattern(4, 12) >> block_pattern(4, false, triangle_shape::upper) >>
        stride_pattern(4, 12);

#pragma omp master
    {
        roco2::log::info() << "Experiment list has " << on_list.size() << " entries: " << on_list;
    }

#pragma omp barrier

    auto experiment_startpoint = roco2::initialize::thread(starting_point, experiment_duration);

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    // do one full idle
    exp.run(idle, roco2::experiments::cpu_sets::all_cpus());

    // do on firestarter with half cycle
    ddcm.change(8);

    exp.run(fs, roco2::experiments::cpu_sets::all_cpus());

    ddcm.disable();

    for (const auto& freq : freq_list)
    {
        freqctl.change(freq);

        for (const auto& on : on_list)
        {

            for (auto cycles : { 1, 5, 10, 15, 16 })
            {

                ddcm.change(cycles);
                // exp.run(hl1, on);
                // exp.run(hl2, on);
                // exp.run(hl3, on);

                exp.run(bw, on);
                exp.run(cp, on);
                exp.run(sinus, on);
                exp.run(mem, on);
                exp.run(addpd, on);
                exp.run(mulpd, on);
                exp.run(squareroot, on);
                exp.run(mm, on);
                exp.run(fs, on);
            }
        }
    }
}

int main(int argc, char** argv)
{
    auto starting_point = std::chrono::high_resolution_clock::now();

    roco2::log::info() << "Starting initialize at: " << starting_point.time_since_epoch().count();

    // first assumption:
    // GOMP_CPU_AFFINITY or respectivly KMP_AFFINITY was set to a sane value.

    if (argc == 2 && argv[1] == std::string("--debug"))
    {
        roco2::logging::filter<roco2::logging::record>::set_severity(
            nitro::log::severity_level::debug);
    }
    else
    {
        roco2::logging::filter<roco2::logging::record>::set_severity(
            nitro::log::severity_level::info);
    }

    omp_set_dynamic(0);

    roco2::log::info() << "Starting with " << omp_get_max_threads() << " threads.";

    auto& topology = roco2::cpu::topology::instance();

    (void)topology;

    roco2::initialize::master();

    bool exception_happend = false;

#pragma omp parallel default(shared) shared(starting_point, exception_happend)
    {
        try
        {
            run_experiments(starting_point);
        }
        catch (std::exception& e)
        {
            roco2::log::fatal() << "An exception happend during execution: " << e.what();
            exception_happend = true;
        }
    }

    if (!exception_happend)
        roco2::log::info() << "Successfully set your machine on fire.";

    return 0;
}
