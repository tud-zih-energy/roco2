#ifndef INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP
#define INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

SCOREP_USER_METRIC_EXTERNAL(experiment_metric)

namespace roco2
{
namespace metrics
{

    class experiment
    {
        experiment()
        {
            SCOREP_USER_METRIC_INIT(experiment_metric, "Experiment", "#",
                                    SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
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

        void write(std::uint64_t value)
        {
            SCOREP_USER_METRIC_UINT64(experiment_metric, value)
            (void)value;
        }
    };
}
}

#endif // INCLUDE_ROCO2_METRICS_EXPERIMENT_HPP
