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
    using firestarter_function = int (*)(void*);

    void* firestartet_data(unsigned long long loop_count,
                           std::vector<double, AlignmentAllocator<double, 32>>& my_mem_buffer)
    {
        threaddata_t* data = (threaddata_t*)malloc(sizeof(threaddata_t));
        data->addrMem = reinterpret_cast<unsigned long long>(my_mem_buffer.data());
        data->addrHigh = loop_count;
        return (void*)data;
    }

    void free_firestartet_data(void* data)
    {
        free(data);
    }

    firestarter_function init_firestarter_kernel()
    {
        int base_function = select_base_function();

        assert(base_function != FUNC_UNKNOWN);

        firestarter_function firestarter_init =
            reinterpret_cast<int (*)(void*)>(get_init_function(base_function, 2));
        firestarter_function firestarter_kernel =
            reinterpret_cast<int (*)(void*)>(get_working_function(base_function, 2));

        assert(firestarter_init != nullptr);
        assert(firestarter_kernel != nullptr);

        auto mem_size = get_memory_size(base_function, 2);
        (void)mem_size;

        auto& my_mem_buffer = roco2::thread_local_memory().firestarter_buffer;

        assert(mem_size / sizeof(double) <= my_mem_buffer.size());

        threaddata_t data;

        data.addrMem = reinterpret_cast<unsigned long long>(my_mem_buffer.data());

        firestarter_init(&data);

        return firestarter_kernel;
    }
}
}
