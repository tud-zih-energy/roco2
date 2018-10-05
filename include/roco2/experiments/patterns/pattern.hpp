#ifndef INCLUDE_ROCO2_PATTERNS_PATTERN_HPP
#define INCLUDE_ROCO2_PATTERNS_PATTERN_HPP

#include <roco2/experiments/cpu_sets/cpu_set.hpp>

#include <iostream>
#include <vector>

namespace roco2
{
namespace experiments
{

    namespace patterns
    {

        class pattern
        {
        public:
            typedef std::vector<cpu_sets::cpu_set> container;
            typedef container::const_iterator const_iterator;

            pattern() = default;

            pattern(const cpu_sets::cpu_set& r) : data(1, r)
            {
            }

            void append(cpu_sets::cpu_set r)
            {
                if (r.num_threads() > 0)
                    data.push_back(r);
            }

            std::vector<cpu_sets::cpu_set>& operator*()
            {
                return data;
            }

            const std::vector<cpu_sets::cpu_set>& operator*() const
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

            operator std::vector<cpu_sets::cpu_set>() const
            {
                return data;
            }

            std::size_t size() const
            {
                return data.size();
            }

        private:
            std::vector<cpu_sets::cpu_set> data;
        };

        inline std::ostream& operator<<(std::ostream& s, const pattern& r)
        {
            for (const auto& i : r)
            {
                s << "\n" << i;
            }
            return s;
        }

        inline pattern operator|(const cpu_sets::cpu_set& range, const pattern& set)
        {
            pattern result;

            for (auto r : set)
            {
                result.append(r | range);
            }

            return result;
        }

        inline pattern operator|(const pattern& set, const cpu_sets::cpu_set& range)
        {
            return range | set;
        }

        inline pattern operator&(const cpu_sets::cpu_set& range, const pattern& set)
        {
            pattern result;

            for (auto r : set)
            {
                result.append(r & range);
            }

            return result;
        }

        inline pattern operator&(const pattern& set, const cpu_sets::cpu_set& range)
        {
            return range & set;
        }

        inline pattern operator^(const cpu_sets::cpu_set& range, const pattern& set)
        {
            pattern result;

            for (auto r : set)
            {
                result.append(r ^ range);
            }

            return result;
        }

        inline pattern operator^(const pattern& set, const cpu_sets::cpu_set& range)
        {
            return range ^ set;
        }

        inline pattern operator>>(pattern a, const pattern& b)
        {
            for (auto range : b)
            {
                a.append(range);
            }

            return a;
        }
    }
}
}
#endif // INCLUDE_ROCO2_PATTERNS_PATTERN_HPP
