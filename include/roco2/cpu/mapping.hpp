#pragma once

#include <roco2/cpu/info.hpp>
#include <roco2/cpu/topology.hpp>

#include <map>
#include <string>

namespace roco2
{
namespace cpu
{
    class mapping
    {
    public:
        static mapping& instance()
        {
            static mapping i;
            return i;
        }

        void initialize(const std::string& ignore_pattern)
        {
            auto ignored_cpus = roco2::cpu::detail::parse_list(ignore_pattern);
            auto available_cpus = roco2::cpu::topology::instance().present_cpus();

            unsigned int thread = 0;

            for (auto cpu : available_cpus)
            {
                if (ignored_cpus.count(cpu))
                    continue;

                log::info() << "mapping thread " << thread << " to cpu " << cpu;

                thread_to_cpu_[thread] = cpu;
                thread++;
            }
        }

        unsigned int cpu_for_thread(unsigned int thread) const
        {
            if (thread_to_cpu_.count(thread))
                return thread_to_cpu_.at(thread);
            else
                return -1;
        }

    private:
        std::map<unsigned int, unsigned int> thread_to_cpu_;
    };
} // namespace cpu
} // namespace roco2
