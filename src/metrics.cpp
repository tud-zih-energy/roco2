// #ifndef SCOREP_USER_ENABLE
// #define SCOREP_USER_ENABLE
// #endif

// #include <roco2/scorep.hpp>
//
// SCOREP_USER_METRIC_GLOBAL(experiment_metric)
// SCOREP_USER_METRIC_GLOBAL(thread_metric)
// SCOREP_USER_METRIC_GLOBAL(frequency_metric)
// SCOREP_USER_METRIC_GLOBAL(ddcm_metric)
// SCOREP_USER_METRIC_GLOBAL(c_state_limit_metric)
// SCOREP_USER_METRIC_GLOBAL(utility_metric)

#include <roco2/metrics/experiment.hpp>
#include <roco2/metrics/frequency.hpp>
#include <roco2/metrics/threads.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/metrics/shell.hpp>
#include <roco2/metrics/meta.hpp>

namespace roco2
{
namespace metrics
{
    void __attribute__((optimize("O0"))) experiment::write(std::uint64_t value)
    {
        // SCOREP_USER_METRIC_UINT64(experiment_metric, value)
        (void)value;
        meta::instance().experiment = value;
        // written last, trigger here
        meta::instance().trigger();
    }

    void __attribute__((optimize("O0"))) frequency::write(std::uint64_t value)
    {
        // SCOREP_USER_METRIC_UINT64(frequency_metric, value)
        (void)value;
        meta::instance().frequency = value;
    }

    void __attribute__((optimize("O0"))) threads::write(std::uint64_t value)
    {
        // SCOREP_USER_METRIC_UINT64(thread_metric, value)
        (void)value;
        meta::instance().threads = value;
    }

    void __attribute__((optimize("O0"))) utility::write(std::uint64_t value)
    {
        // SCOREP_USER_METRIC_UINT64(utility_metric, value)
        (void)value;
    }

    void __attribute__((optimize("O0"))) shell::write(std::int64_t value)
    {
        // SCOREP_USER_METRIC_UINT64(shell_metric, value)
        (void)value;
        meta::instance().shell = value;
    }

    void __attribute__((optimize("O0"))) meta::write(std::uint64_t experiment, std::uint64_t frequency, std::int64_t shell,
                                                     std::uint64_t threads)
    {
        (void)experiment;
        (void)frequency;
        (void)shell;
        (void)threads;
    }
} // namespace metrics
} // namespace roco2
