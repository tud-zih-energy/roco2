#ifndef INCLUDE_ROCO2_METRICS_UTILITY_HPP
#define INCLUDE_ROCO2_METRICS_UTILITY_HPP

#include <cstdint>
// #include <roco2/scorep.hpp>

// SCOREP_USER_METRIC_EXTERNAL(utility_metric)

namespace roco2
{
namespace metrics
{

    class utility
    {
        utility()
        {
            // SCOREP_USER_METRIC_INIT(utility_metric, "Utility", "#",
            // SCOREP_USER_METRIC_TYPE_UINT64,
            //                         SCOREP_USER_METRIC_CONTEXT_GLOBAL)
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

        void __attribute__((optimize("O0"))) write(std::uint64_t value)
        {
            // SCOREP_USER_METRIC_UINT64(utility_metric, value)
            (void)value;
        }
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_UTILITY_HPP
