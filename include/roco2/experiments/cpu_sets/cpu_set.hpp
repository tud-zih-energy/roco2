#ifndef INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP
#define INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP

#include <omp.h>

#include <bitset>
#include <iostream>
#include <set>
#include <vector>

#include <cassert>

namespace roco2
{
namespace experiments
{

    namespace cpu_sets
    {
        class cpu_set
        {
        public:
            static constexpr std::size_t MAX_CPUS = 1024;
            using container = std::bitset<MAX_CPUS>;

            cpu_set() = default;

            cpu_set(const std::set<std::size_t>& range)
            {
                for (auto cpu : range)
                    add(cpu);
            }

            void add(std::size_t cpu_id)
            {
                if (static_cast<int>(cpu_id) < omp_get_max_threads())
                    data.set(cpu_id);
            }

            void remove(std::size_t cpu_id)
            {
                data.reset(cpu_id);
            }

            bool contains(std::size_t cpu_id) const
            {
                return data.test(cpu_id);
            }

            std::size_t num_threads() const
            {
                return data.count();
            }

            std::size_t max() const
            {
                // TODO
                return MAX_CPUS;
            }

        //private:
            container data;
        };

        inline std::ostream& operator<<(std::ostream& s, const cpu_set& r)
        {
            s << "[";
            for (std::size_t cpu = 0; cpu < r.max(); cpu++)
            {
                s << (r.contains(cpu) ? "#" : ".");
            }
            return s << "]";
        }

        inline cpu_set operator|(cpu_set a, const cpu_set& b)
        {
            a.data |= b.data;
            return a;
        }

        inline cpu_set operator&(cpu_set a, const cpu_set& b)
        {
            a.data &= b.data;
            return a;
        }

        inline cpu_set operator^(cpu_set a, const cpu_set& b)
        {
            a.data ^= b.data;
            return a;
        }
    }
}
}

#endif // INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP
