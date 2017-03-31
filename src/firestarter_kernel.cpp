#include <roco2/kernels/firestarter.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>

#include <roco2/scorep.hpp>

#include <cassert>

extern "C" {
#include <firestarter.h>
}

namespace roco2
{
namespace kernels
{

    firestarter::firestarter()
    {
        base_function_ = select_base_function();

        assert(base_function_ != FUNC_UNKNOWN);

        firestarter_init_ = reinterpret_cast<int (*)(void*)>(get_init_function(base_function_, 2));
        firestarter_function_ =
            reinterpret_cast<int (*)(void*)>(get_working_function(base_function_, 2));

        assert(firestarter_init_ != nullptr);
        assert(firestarter_function_ != nullptr);

        auto mem_size = get_memory_size(base_function_, 2);
        (void)mem_size;

        auto& my_mem_buffer = roco2::thread_local_memory().firestarter_buffer;

        assert(mem_size <= roco2::thread_local_memory::firestarter_size * sizeof(my_mem_buffer[0]));

        threaddata_t data;

        data.addrMem = reinterpret_cast<param_type>(my_mem_buffer.data());

        firestarter_init_(&data);
    }

    void firestarter::run_kernel(roco2::chrono::time_point until)
    {
        SCOREP_USER_REGION("firestarter_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

        assert(firestarter_function_);

        auto& my_mem_buffer = roco2::thread_local_memory().firestarter_buffer;

        // check alignment requirements
        assert(reinterpret_cast<param_type>(my_mem_buffer.data()) % 32 == 0);

        threaddata_t data;
        data.addrMem = reinterpret_cast<param_type>(my_mem_buffer.data());
        data.addrHigh = loop_count;

        std::size_t loops = 0;

        do
        {
            // SCOREP_USER_REGION("firestarter_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)
            firestarter_function_(&data);

            loops++;
        } while (std::chrono::high_resolution_clock::now() < until);

        roco2::metrics::utility::instance().write(loops);
    }
}
}
