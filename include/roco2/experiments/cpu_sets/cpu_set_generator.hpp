#ifndef INCLUDE_ROCO2_PATTERNS_CPU_SET_GENERATOR_HPP
#define INCLUDE_ROCO2_PATTERNS_CPU_SET_GENERATOR_HPP

#include <roco2/experiments/cpu_sets/cpu_set.hpp>
#include <roco2/log.hpp>

#include <roco2/cpu/topology.hpp>

#include <set>
#include <vector>

namespace roco2
{
namespace experiments
{
    namespace cpu_sets
    {

        inline cpu_set make_cpu_set_from(const roco2::cpu::topology::socket& socket)
        {
            cpu_set result;

            for (auto& core : socket.cores)
            {
                result.add(core);
            }

            return result;
        }

        inline cpu_set make_cpu_range(std::size_t from = 0, std::size_t to = 0,
                                      std::size_t stride = 1)
        {
            cpu_set result;

            for (std::size_t i = from; i <= to; i += stride)
            {
                result.add(i);
            }

            return result;
        }

        inline cpu_set even_cpus()
        {
            return make_cpu_range(0, cpu::topology::instance().num_cores(), 2);
        }

        inline cpu_set odd_cpus()
        {
            return make_cpu_range(1, cpu::topology::instance().num_cores(), 2);
        }

        inline cpu_set cpu(std::size_t id)
        {
            if (id >= cpu::topology::instance().num_cores())
            {
                roco2::log::warn() << "There is no cpu with id: " << id;
            }

            return make_cpu_range(id, id);
        }

        inline cpu_set all_cpus()
        {
            return make_cpu_range(0, cpu::topology::instance().num_cores() - 1);
        }
    }
}
}

#endif // INCLUDE_ROCO2_PATTERNS_CPU_SET_GENERATOR_HPP
