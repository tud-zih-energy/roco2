#ifndef INCLUDE_ROCO2_METRICS_DDCM_HPP
#define INCLUDE_ROCO2_METRICS_DDCM_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(ddcm_metric)
#endif

namespace roco2
{
namespace metrics
{

    class ddcm
    {
        ddcm()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(ddcm_metric, "Dynamic Duty Cycles", "%",
                                    SCOREP_USER_METRIC_TYPE_DOUBLE,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
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

        void __attribute__((optimize("O0"))) write(double value);
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_DDCM_HPP
