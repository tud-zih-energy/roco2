#ifndef INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP
#define INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP

#include <omp.h>

#include <iostream>
#include <set>
#include <vector>

namespace roco2
{
namespace experiments
{

    namespace cpu_sets
    {
        class cpu_set
        {
        public:
            typedef std::set<std::size_t> container;
            typedef container::const_iterator const_iterator;

            cpu_set() = default;

            cpu_set(const std::set<std::size_t>& range)
            {
                for (auto cpu : range)
                    add(cpu);
            }

            void add(std::size_t cpu_id)
            {
                if (static_cast<int>(cpu_id) < omp_get_max_threads())
                    data.insert(cpu_id);
            }

            void remove(std::size_t cpu_id)
            {
                data.erase(cpu_id);
            }

            bool contains(std::size_t cpu_id) const
            {
                return data.count(cpu_id);
            }

            container& operator*()
            {
                return data;
            }

            const container& operator*() const
            {
                return data;
            }

            const_iterator begin() const
            {
                return data.begin();
            }

            const_iterator end() const
            {
                return data.end();
            }

            std::size_t num_threads() const
            {
                return data.size();
            }

            std::size_t max() const
            {
                if (data.empty())
                {
                    return 0;
                }
                return *data.rbegin() + 1;
            }

        private:
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
            for (auto cpu_id : b)
            {
                a.add(cpu_id);
            }

            return a;
        }

        inline cpu_set operator&(const cpu_set& a, const cpu_set& b)
        {
            cpu_set res;

            for (auto cpu_id : a)
            {
                if (b.contains(cpu_id))
                {
                    res.add(cpu_id);
                }
            }

            return res;
        }

        inline cpu_set operator^(const cpu_set& a, const cpu_set& b)
        {
            cpu_set res(a);

            for (auto cpu_id : b)
            {
                if (a.contains(cpu_id))
                {
                    res.remove(cpu_id);
                }
                else
                {
                    res.add(cpu_id);
                }
            }

            return res;
        }

        inline cpu_set operator-(cpu_set a, const cpu_set& b)
        {
            for (auto cpu_id : b)
            {
                a.remove(cpu_id);
            }

            return a;
        }
    }
}
}

#endif // INCLUDE_ROCO2_PATTERNS_CPU_RANGE_HPP
