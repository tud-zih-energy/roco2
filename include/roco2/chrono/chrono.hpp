#ifndef INCLUDE_ROCO2_CHRONO_CHRONO_HPP
#define INCLUDE_ROCO2_CHRONO_CHRONO_HPP

#include <roco2/chrono/util.hpp>

#include <chrono>
#include <iostream>

namespace roco2
{
namespace chrono
{
    using time_point = std::chrono::high_resolution_clock::time_point;
    using duration = time_point::duration;

    inline time_point now()
    {
        return std::chrono::high_resolution_clock::now();
    }
}
}

namespace std
{
namespace chrono
{

    // HACK: BE AWARE OF DEAD KITTENS

    inline std::ostream& operator<<(std::ostream& s, nanoseconds dur)
    {
        return s << dur.count() << "ns";
    }

    inline std::ostream& operator<<(std::ostream& s, microseconds dur)
    {
        return s << dur.count() << "us";
    }

    inline std::ostream& operator<<(std::ostream& s, milliseconds dur)
    {
        return s << dur.count() << "ms";
    }

    inline std::ostream& operator<<(std::ostream& s, seconds dur)
    {
        return s << dur.count() << "s";
    }

    inline std::ostream& operator<<(std::ostream& s, minutes dur)
    {
        return s << dur.count() << "min";
    }

    inline std::ostream& operator<<(std::ostream& s, hours dur)
    {
        return s << dur.count() << "h";
    }
}
} // namespace std::chrono

#endif // INCLUDE_ROCO2_CHRONO_CHRONO_HPP
