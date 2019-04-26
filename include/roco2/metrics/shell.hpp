#pragma once

#include <cstdint>
#include <roco2/scorep.hpp>

#ifdef HAS_SCOREP
SCOREP_USER_METRIC_EXTERNAL(shell_metric)
#endif

namespace roco2
{
namespace metrics
{
    class shell
    {
        shell()
        {
#ifdef HAS_SCOREP
            SCOREP_USER_METRIC_INIT(shell_metric, "generic_shell_metric", "#",
                                    SCOREP_USER_METRIC_TYPE_INT64,
                                    SCOREP_USER_METRIC_CONTEXT_GLOBAL)
#endif
        }

    public:
        using value_type = std::int64_t;

        shell(const shell&) = delete;
        shell& operator=(const shell&) = delete;

        static shell& instance()
        {
            static shell e;

            return e;
        }

        void write(value_type value);
    };
} // namespace metrics
} // namespace roco2
