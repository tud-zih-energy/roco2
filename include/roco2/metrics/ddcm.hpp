#ifndef INCLUDE_ROCO2_METRICS_DDCM_HPP
#define INCLUDE_ROCO2_METRICS_DDCM_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

SCOREP_USER_METRIC_EXTERNAL(ddcm_metric)

namespace roco2
{
namespace metrics
{

    class ddcm
    {
        ddcm()
        {
            SCOREP_USER_METRIC_INIT(ddcm_metric, "Dynamic Duty Cycles", "%",
                                    SCOREP_USER_METRIC_TYPE_DOUBLE,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
        }

    public:
        using value_type = double;

        ddcm(const ddcm&) = delete;
        ddcm& operator=(const ddcm&) = delete;

        static ddcm& instance()
        {
            static ddcm e;

            return e;
        }

        void write(double value)
        {
            SCOREP_USER_METRIC_DOUBLE(ddcm_metric, value)
            (void)value;
        }
    };
}
}

#endif // INCLUDE_ROCO2_METRICS_DDCM_HPP
