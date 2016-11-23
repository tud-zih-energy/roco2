#ifndef INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP
#define INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

SCOREP_USER_METRIC_EXTERNAL(c_state_limit_metric)

namespace roco2
{
namespace metrics
{

    class c_state_limit
    {
        c_state_limit()
        {
            SCOREP_USER_METRIC_INIT(c_state_limit_metric, "c_state_limit", "#",
                                    SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
        }

    public:
        using value_type = std::uint64_t;

        c_state_limit(const c_state_limit&) = delete;
        c_state_limit& operator=(const c_state_limit&) = delete;

        static c_state_limit& instance()
        {
            static c_state_limit e;

            return e;
        }

        void write(std::uint64_t value)
        {
            SCOREP_USER_METRIC_UINT64(c_state_limit_metric, value)
            (void)value;
        }
    };
}
}

#endif // INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP
