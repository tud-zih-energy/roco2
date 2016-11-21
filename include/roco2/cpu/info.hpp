#ifndef INCLUDE_ROCO2_CPU_INFO_HPP
#define INCLUDE_ROCO2_CPU_INFO_HPP

#include <roco2/exception.hpp>
#include <roco2/log.hpp>

#include <omp.h>
#include <sched.h>

#include <cstdlib>

namespace roco2
{

namespace cpu
{
    class info
    {
    public:
        static unsigned int current_cpu()
        {
            int result = sched_getcpu();

            if (result == -1)
            {
                raise("Couldn't get id of current cpu");
            }

            return result;
        }

        static unsigned int current_thread()
        {
            int result = omp_get_thread_num();

            if (result == -1)
            {
                raise("Couldn't get id of current thread");
            }

            return result;
        }
    };
}
}

#endif // INCLUDE_ROCO2_CPU_INFO_HPP
