#ifndef INCLUDE_ROCO2_METRICS_THREADS_HPP
#define INCLUDE_ROCO2_METRICS_THREADS_HPP

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(thread_metric)
#endif

namespace roco2
{
namespace metrics
{

    class threads
    {
        threads()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(thread_metric, "Threads", "#", SCOREP_USER_METRIC_TYPE_UINT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
        }

    public:
        using value_type = std::uint64_t;

        threads(const threads&) = delete;
        threads& operator=(const threads&) = delete;

        static threads& instance()
        {
            static threads e;

            return e;
        }

        void write(std::uint64_t value);
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_THREADS_HPP
