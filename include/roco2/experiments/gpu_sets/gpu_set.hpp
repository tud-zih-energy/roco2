
#pragma once


#include <bitset>
#include <cstddef>
#include <iostream>
#include <set>

#include <cassert>

namespace roco2
{
namespace experiments
{

    namespace gpu_sets
    {
        class gpu_set
        {
        public:
            static constexpr std::size_t MAX_GPUS = 32;
            using container = std::bitset<MAX_GPUS>;

            gpu_set() = default;

            gpu_set(const std::set<std::size_t>& range)
            {
                for (auto gpu : range)
                    add(gpu);
            }

            void add(std::size_t gpu_id)
            {
                data.set(gpu_id);
            }

            void remove(std::size_t gpu_id)
            {
                data.reset(gpu_id);
            }

            bool contains(std::size_t gpu_id) const
            {
                return data.test(gpu_id);
            }

            std::size_t max() const {
                return MAX_GPUS;
            }

        //private:
            container data;
        };

        inline std::ostream& operator<<(std::ostream& s, const gpu_set& r)
        {
            s << "[";
            for (std::size_t gpu = 0; gpu < r.max(); gpu++)
            {
                s << (r.contains(gpu) ? "#" : ".");
            }
            return s << "]";
        }

        inline gpu_set operator|(gpu_set a, const gpu_set& b)
        {
            a.data |= b.data;
            return a;
        }

        inline gpu_set operator&(gpu_set a, const gpu_set& b)
        {
            a.data &= b.data;
            return a;
        }

        inline gpu_set operator^(gpu_set a, const gpu_set& b)
        {
            a.data ^= b.data;
            return a;
        }
    }
}
}

