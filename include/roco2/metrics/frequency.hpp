#ifndef INCLUDE_ROCO2_METRICS_FREQUENCY_HPP
#define INCLUDE_ROCO2_METRICS_FREQUENCY_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(frequency_metric)
#endif

namespace roco2
{
namespace metrics
{

    class frequency
    {
        frequency()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(frequency_metric, "Frequency", "Hz",
                                    SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
        }

    public:
        using value_type = std::uint64_t;

        frequency(const frequency&) = delete;
        frequency& operator=(const frequency&) = delete;

        static frequency& instance()
        {
            static frequency e;

            return e;
        }

        void write(std::uint64_t value);
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_FREQUENCY_HPP
