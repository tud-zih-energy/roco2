#ifndef INCLUDE_ROCO2_CHRONO_UITL_HPP
#define INCLUDE_ROCO2_CHRONO_UITL_HPP

#include <chrono>

namespace roco2
{
namespace chrono
{

    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

    inline void busy_wait_until(time_point end)
    {
        while (std::chrono::high_resolution_clock::now() < end)
        {
        }
    }

    template <typename Rep, typename Period>
    inline void busy_wait_for(std::chrono::duration<Rep, Period> dur)
    {
        auto then = std::chrono::high_resolution_clock::now() + dur;

        busy_wait_until(then);
    }
}
}

#endif // INCLUDE_ROCO2_CHRONO_UITL_HPP
