#ifndef INCLUDE_ROCO2_DDCM_HPP
#define INCLUDE_ROCO2_DDCM_HPP

#include <roco2/cpu/info.hpp>
#include <roco2/exception.hpp>
#include <roco2/log.hpp>
#include <roco2/metrics/ddcm.hpp>

#ifdef HAS_X86ADAPT
#include <x86_adapt.h>
#endif

#include <cassert>
#include <cstring>
#include <ios>
#include <string>

using namespace std::string_literals;

namespace roco2
{

namespace cpu
{
    class ddcm
    {
    public:
        ddcm()
        {
#ifdef HAS_X86ADAPT
            auto result = x86_adapt_init();

            if (result)
            {
                raise("Couldn't initialize x86_adapt: ", result);
            }

            auto nr_cis = x86_adapt_get_number_cis(X86_ADAPT_CPU);
            struct x86_adapt_configuration_item item;
            index_value = -1;
            for (int j = 0; j < nr_cis; j++)
            {
                x86_adapt_get_ci_definition(X86_ADAPT_CPU, j, &item);

                if ("Intel_Clock_Modulation"s == item.name)
                {
                    index_value = j;
                }
            }

            if (index_value == -1)
            {
                raise("Could not find x86_adapt CIs for DDCM knobs.");
            }

            if (static_cast<int>(roco2::cpu::info::current_cpu()) >=
                x86_adapt_get_nr_avaible_devices(X86_ADAPT_CPU))
            {
                log::warn() << "Something fishy is going on.";
                log::warn() << "Current cpu id is bigger than x86_adapt would expect.";
            }

            log::info() << "DDCM initialized successfully.";

            disable();
#endif
        }

        ~ddcm()
        {
#ifdef HAS_X86ADAPT

            disable();

#pragma omp barrier
#pragma omp master
            {
                x86_adapt_finalize();
            }
#endif
        }

        ddcm(const ddcm&) = delete;
        ddcm& operator=(const ddcm&) = delete;

        /**
         * Tries to set the new duty cycle rate
         *
         * @param new_freq Duty cycle rate from 1=6.25 to 16=full speed (disabled)
         */
        void change(std::uint64_t new_ddcm_value)
        {
            assert(new_ddcm_value > 0);
            assert(new_ddcm_value <= 16);
#ifdef HAS_X86ADAPT

            int value = new_ddcm_value == 16 ? 0x0 : (new_ddcm_value | 0x10);

            auto cpuid = roco2::cpu::info::current_cpu();

            int fd = x86_adapt_get_device(X86_ADAPT_CPU, cpuid);
            if (fd < 0)
            {
                log::error() << "Could not get x86_adapt device for CPU " << cpuid << ", error "
                             << fd;
                return;
            }

            auto ret = x86_adapt_set_setting(fd, index_value, value);
            if (ret < 0)
            {
                log::error() << "Could not set x86_adapt setting for CPU " << cpuid << ", error "
                             << ret << ", new value 0x" << std::hex << value;

                return;
            }

            ret = x86_adapt_put_device(X86_ADAPT_CPU, cpuid);
            if (ret < 0)
            {
                log::error() << "Could not put x86_adapt device for CPU " << cpuid << ", error "
                             << ret;
                return;
            }

            roco2::metrics::ddcm::instance().write(new_ddcm_value * 6.25);
#else
#pragma omp master
            {
                log::warn() << "DDCM change to " << new_ddcm_value
                            << " requested, but roco2 was built without X86Adapt.";
            }
#endif
        }

        void disable()
        {
#ifdef HAS_X86ADAPT
            change(16);
#endif
        }

    private:
        int index_value;
    };
}
}
#endif // INCLUDE_ROCO2_DDCM_HPP
