#ifndef INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
#define INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <roco2/chrono/util.hpp>

extern "C" {
#include <firestarter.h>
}

#include <cassert>

namespace roco2
{
namespace kernels
{

    class firestarter : public base_kernel
    {

        using param_type = unsigned long long;

    public:
        firestarter()
        {
            firestarter_init();

            firestarter_init_ = get_firestarter_init();
            firestarter_function_ = get_firestarter_loop();

            // auto& my_mem_buffer = roco2::thread_local_memory().mem_buffer;

            // assert(firestarter_init_);
            // firestarter_init_(reinterpret_cast<param_type>(my_mem_buffer.data()));
        }

        virtual experiment_tag tag() const override
        {
            return 6;
        }

    private:
        void run_kernel(roco2::chrono::time_point until) override
        {
            SCOREP_USER_REGION("firestarter_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            assert(firestarter_function_);

            auto& my_mem_buffer = roco2::thread_local_memory().firestarter_buffer;

            // check alignment requirements
            assert(reinterpret_cast<param_type>(my_mem_buffer.data()) % 32 == 0);

            std::size_t loops = 0;

            do
            {
                // SCOREP_USER_REGION("firestarter_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)
                firestarter_function_(reinterpret_cast<param_type>(my_mem_buffer.data()),
                                      loop_count);

                loops++;
            } while (std::chrono::high_resolution_clock::now() < until);

            roco2::metrics::utility::instance().write(loops);
        }

        int (*firestarter_function_)(param_type, param_type);
        int (*firestarter_init_)(param_type);

        const static param_type loop_count = 10000;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
