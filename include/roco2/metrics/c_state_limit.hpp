#ifndef INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP
#define INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(c_state_limit_metric)
#endif

namespace roco2
{
namespace metrics
{

    class c_state_limit
    {
        c_state_limit()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(c_state_limit_metric, "c_state_limit", "#",
                                    SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
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

        void __attribute__((optimize("O0"))) write(std::uint64_t value)
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_UINT64(c_state_limit_metric, value)
#endif

            (void)value;
        }
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_C_STATE_LIMIT_HPP
