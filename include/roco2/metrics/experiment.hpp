#ifndef INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP
#define INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(experiment_metric)
#endif

namespace roco2
{
namespace metrics
{

    class experiment
    {
        experiment()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(experiment_metric, "Experiment", "#",
                                    SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
        }

    public:
        using value_type = std::uint64_t;

        experiment(const experiment&) = delete;
        experiment& operator=(const experiment&) = delete;

        static experiment& instance()
        {
            static experiment e;

            return e;
        }

        void write(std::uint64_t value);
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP
