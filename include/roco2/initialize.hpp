#ifndef INCLUDE_ROCO2_INITIALIZE_HPP
#define INCLUDE_ROCO2_INITIALIZE_HPP

#include <roco2/chrono/chrono.hpp>
#include <roco2/cpu/affinity.hpp>
#include <roco2/cpu/info.hpp>
#include <roco2/cpu/topology.hpp>
#include <roco2/log.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/experiment.hpp>
#include <roco2/metrics/threads.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <omp.h>

#include <thread>
#include <vector>

namespace roco2
{

class initialize
{
public:
    static void master()
    {
        SCOREP_USER_REGION("master_init", SCOREP_USER_REGION_TYPE_FUNCTION)

        // initialize the cpu topology structure
        auto& topology = roco2::cpu::topology::instance();
        (void)topology;

        roco2::metrics::experiment::instance().write(1);
        roco2::metrics::threads::instance().write(1);

        // first call to thread_local_memory which initializes the static
        // variable there. So this must be called once, while only master is
        // active
        thread_local_memory();

#ifdef ROCO2_ASSERTIONS
        log::warn() << "Additional runtime checks enabled.";
        log::warn() << "This may influence or corrupt your measurement.";
#endif
    }

    static roco2::chrono::time_point thread(roco2::chrono::time_point start_point,
                                            roco2::chrono::duration experiment_duration,
                                            bool eta_only)
    {
        SCOREP_USER_REGION("thread_init", SCOREP_USER_REGION_TYPE_FUNCTION)

#pragma omp barrier

        bool is_master = false;

#pragma omp master
        {
            is_master = true;
        }

        if (eta_only)
        {
            return start_point + experiment_duration;
        }

        if (!is_master)
        {
            roco2::metrics::experiment::instance().write(1);
            roco2::metrics::threads::instance().write(omp_get_num_threads());
        }

        log::info() << "Checking affinity of thread to correct cpu";
        if (cpu::info::current_cpu() != cpu::info::current_thread())
        {
            raise("Thread ", cpu::info::current_thread(), " is on wrong cpu: ",
                  cpu::info::current_cpu());
        }

        thread_local_memory();

        log::info() << "Allocated and first touched memory";

        log::info() << "Waiting for nice starting point";

        auto then = start_point + experiment_duration;

#pragma omp barrier

        auto now = roco2::chrono::now();

        while (now >= then)
        {
            then += experiment_duration;
            now = roco2::chrono::now();
        }

        log::info() << "Starting at: " << then.time_since_epoch().count();

        std::this_thread::sleep_until(then);

        roco2::metrics::utility::instance().write(1);
        roco2::metrics::experiment::instance().write(1);

        return then;
    }
};
}

#endif // INCLUDE_ROCO2_INITIALIZE_HPP
