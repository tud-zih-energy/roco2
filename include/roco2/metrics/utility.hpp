#ifndef INCLUDE_ROCO2_METRICS_UTILITY_HPP
#define INCLUDE_ROCO2_METRICS_UTILITY_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(utility_metric)
#endif

namespace roco2
{
namespace metrics
{

    class utility
    {
        utility()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(utility_metric, "Utility", "#", SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
        }

    public:
        using value_type = std::uint64_t;

        utility(const utility&) = delete;
        utility& operator=(const utility&) = delete;

        static utility& instance()
        {
            static utility e;

            return e;
        }

        void write(std::uint64_t value);
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_UTILITY_HPP
