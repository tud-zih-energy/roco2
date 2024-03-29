#ifndef INCLUDE_ROCO2_CPU_FREQUENCY_HPP
#define INCLUDE_ROCO2_CPU_FREQUENCY_HPP

#ifndef HAS_CPUFREQ
#error CpuFreq is required to include this file
#endif // HAS_CPUFREQ

#include <roco2/cpu/info.hpp>
#include <roco2/cpu/topology.hpp>
#include <roco2/exception.hpp>
#include <roco2/log.hpp>
#include <roco2/metrics/frequency.hpp>

extern "C"
{
#include <cpufreq.h>
#include <fastcpufreq.h>
}

#include <cstdlib>
#include <set>
#include <string>

namespace roco2
{

namespace cpu
{
    class frequency
    {
    public:
        frequency()
        : start_frequency_(cpufreq_get_freq_kernel(roco2::cpu::info::current_cpu()) / 1000)
        {
            auto policy = cpufreq_get_policy(roco2::cpu::info::current_cpu());

            governor_ = policy->governor;

            cpufreq_put_policy(policy);

            auto frequencies = cpufreq_get_available_frequencies(roco2::cpu::info::current_cpu());

            auto p_freq = frequencies;

            if (frequencies == nullptr)
            {
                raise("Couldn't read list of available frequencies.");
            }

            while (p_freq)
            {
                valid_frequencies_.insert(p_freq->frequency / 1000);
                p_freq = p_freq->next;
            }

            cpufreq_put_available_frequencies(frequencies);

#pragma omp barrier
#pragma omp master
            {
                std::vector<int> core_status(topology::instance().cores().size());

                for (auto core : topology::instance().cores())
                {
                    core_status[core.id] = core.online ? 1 : 0;
                }

                int result = fcf_init_once(core_status.data(), core_status.size());

                if (result < 0)
                {
                    raise("Couldn't initialize fastcpufreq");
                }
            }
#pragma omp barrier

            if (start_frequency_ == 0)
            {
                log::warn() << "Couldn't read initial frequency.";
            }
            else
            {
                roco2::metrics::frequency::instance().write(start_frequency_ * 1000 * 1000);
            }
        }

        ~frequency()
        {
            // restore frequency
            try
            {
                // As it gives problems, we don't restore the old frequency after a run anymore.
                // We may read some random turbo frequency at startup and not the actual requested
                // frequency.
                // change(start_frequency_);
            }
            catch (const std::exception& e)
            {
                log::error() << "Failed to restore original frequency: " << e.what();
            }

#pragma omp barrier
#pragma omp master
            {
                int result = fcf_finalize();

                if (result < 0)
                {
                    log::warn() << "Couldn't finalize fastcpufreq";
                }
            }
#pragma omp barrier

            // restore governor setting
            // the const cast is okay here, the char* is not written in kernel code. See:
            // https://github.com/torvalds/linux/blob/master/tools/power/cpupower/lib/cpufreq.c#L598
            int result = cpufreq_modify_policy_governor(roco2::cpu::info::current_cpu(),
                                                        const_cast<char*>(governor_.c_str()));

            if (result < 0)
            {
                log::warn() << "Couldn't restore previously used cpufreq governor: " << governor_;
            }
        }

        frequency(const frequency&) = delete;
        frequency& operator=(const frequency&) = delete;

        /**
         * Tries to set the new cpu frequency
         *
         * @param new_freq The new frequency in MHz
         */
        void change(std::uint64_t new_freq)
        {
            if (must_disable_turbo_)
            {
                disable_turbo();
            }
            auto metric_freq = new_freq * 1000 * 1000;
            if (valid_frequencies_.count(new_freq) == 0)
            {
                if (new_freq % 100 == 1)
                {
                    new_freq -= 1;
                    // Enable turbo
                    must_disable_turbo_ = true;
                    enable_turbo();
                }
                else
                {
                    raise("Try to set an invalid frequency: ", new_freq, "MHz");
                }
            }

            int result = fcf_set_frequency(roco2::cpu::info::current_cpu(), new_freq * 1000);

            if (result < 0)
            {
                log::warn() << "Couldn't change frequency for cpu "
                            << roco2::cpu::info::current_cpu() << " to " << new_freq << "MHz";
            }
            else
            {
                roco2::metrics::frequency::instance().write(metric_freq);
            }

#ifdef ROCO2_ASSERTIONS
            verify(new_freq);
#endif
        }

        void disable_turbo()
        {
#pragma omp barrier
#pragma omp master
            {
                auto ret = std::system("elab frequency nominal");
                if (ret)
                {
                    log::warn() << "turbo shell command failed (" << ret << ") ";
                }
                else
                {
                    log::info() << "turbo shell command successful: ";
                }
            }
#pragma omp barrier
        }

        void enable_turbo()
        {
#pragma omp barrier
#pragma omp master
            {
                auto ret = std::system("elab frequency turbo");
                if (ret)
                {
                    log::warn() << "turbo shell command failed (" << ret << ") ";
                }
                else
                {
                    log::info() << "turbo shell command successful: ";
                }
            }
#pragma omp barrier
        }

        /**
         * Checks current frequency with cpufreq against expected value
         *
         * @param expected_freq The expected frequency
         */
        void verify(std::uint64_t expected_freq)
        {
            auto current_freq = cpufreq_get_freq_kernel(roco2::cpu::info::current_cpu()) / 1000;

            if (current_freq != expected_freq)
            {
                raise("Frequency mismatch: current ", current_freq, ", expected: ", expected_freq);
            }
        }

    private:
        std::uint64_t start_frequency_;
        std::string governor_;
        std::set<unsigned long> valid_frequencies_;
        bool must_disable_turbo_ = false;
    };
} // namespace cpu
} // namespace roco2

#endif // INCLUDE_ROCO2_CPU_FREQUENCY_HPP
