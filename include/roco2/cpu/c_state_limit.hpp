#ifndef INCLUDE_ROCO2_CPU_C_STATE_LIMIT_HPP
#define INCLUDE_ROCO2_CPU_C_STATE_LIMIT_HPP

#include <roco2/cpu/info.hpp>
#include <roco2/exception.hpp>
#include <roco2/log.hpp>
#include <roco2/metrics/c_state_limit.hpp>

extern "C" {
#include <c_state_limit.h>
}

#include <cstdlib>
#include <limits>

namespace roco2
{

namespace cpu
{
    class c_state_limit
    {
    public:
        c_state_limit()
        {
#pragma omp master
            {
                int result = csl_init();

                if (result != 0)
                {
                    raise("Couldn't initialize c_state_limits. Error: ", result);
                }
            }
#pragma omp barrier

            disable();
        }

        ~c_state_limit()
        {
            disable();

#pragma omp barrier
#pragma omp master
            {
                int result = csl_fini();

                if (result != 0)
                {
                    log::warn() << "Couldn't finalize c_state_limits";
                }
            }
#pragma omp barrier
        }

        c_state_limit(const c_state_limit&) = delete;
        c_state_limit& operator=(const c_state_limit&) = delete;

        void change(std::uint64_t max_state)
        {
            int result = csl_set_max_cstate(roco2::cpu::info::current_cpu(), max_state);

            if (result != 0)
            {
                log::warn() << "Couldn't change c_state_limits for cpu "
                            << roco2::cpu::info::current_cpu() << " to " << max_state
                            << ". Error: " << result;
            }
            else
            {
                roco2::metrics::c_state_limit::instance().write(max_state * 1000 * 1000);
            }
        }

        int deepest_c_state()
        {
            int cstate;
            int result = csl_get_deepest_cstate(roco2::cpu::info::current_cpu(), &cstate);

            if (result != 0)
            {
                log::warn() << "Couldn't get deepest c-state for cpu "
                            << roco2::cpu::info::current_cpu();

                return std::numeric_limits<int>::max();
            }

            return cstate;
        }

        void disable()
        {
            change(deepest_c_state());
        }

    private:
        std::uint64_t start_frequency_;
        std::string governor_;
    };
}
}

#endif // INCLUDE_ROCO2_CPU_C_STATE_LIMIT_HPP
