#ifndef INCLUDE_ROCO2_METRICS_THREADS_HPP
#define INCLUDE_ROCO2_METRICS_THREADS_HPP

#include <cstdint>
// #include <roco2/scorep.hpp>

// SCOREP_USER_METRIC_EXTERNAL(thread_metric)

namespace roco2
{
namespace metrics
{

    class threads
    {
        threads()
        {
            // SCOREP_USER_METRIC_INIT(thread_metric, "Threads", "#",
            // SCOREP_USER_METRIC_TYPE_UINT64,
            //                         SCOREP_USER_METRIC_CONTEXT_GLOBAL)
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

        void __attribute__((optimize("O0"))) write(std::uint64_t value)
        {
            // SCOREP_USER_METRIC_UINT64(thread_metric, value)
            (void)value;
        }
    };
} // namespace metrics
} // namespace roco2

#endif // INCLUDE_ROCO2_METRICS_THREADS_HPP
