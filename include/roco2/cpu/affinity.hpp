#ifndef INCLUDE_ROCO2_CPU_AFFINITY_HPP
#define INCLUDE_ROCO2_CPU_AFFINITY_HPP

#include <roco2/log.hpp>

#include <sched.h>

#include <vector>

namespace roco2
{

namespace cpu
{
    class affinity
    {
    public:
        static void set(std::size_t cpu_id)
        {
            cpu_set_t mask;

            CPU_ZERO(&mask);

            CPU_SET(cpu_id, &mask);

            int ret = sched_setaffinity(0, sizeof(mask), &mask);

            if (ret != 0)
            {
                log::error() << "Couldn't set cpu affinity";
            }
        }

        static bool isset(std::size_t cpu_id)
        {
            cpu_set_t mask;
            sched_getaffinity(0, sizeof(mask), &mask);

            return CPU_ISSET(cpu_id, &mask);
        }
    };
}
}

#endif // INCLUDE_ROCO2_CPU_AFFINITY_HPP
