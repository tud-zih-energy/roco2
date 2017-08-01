#ifndef INCLUDE_ROCO2_KERNELS_MEMORY_READ_HPP
#define INCLUDE_ROCO2_KERNELS_MEMORY_READ_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{

    template <typename return_functor, std::size_t ChunkSize = 64 * 1024 * 1024>
    class memory_read : public base_kernel<return_functor>
    {
    public:
        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 8;
        }

    private:
        virtual void run_kernel(return_functor& cond) override
        {
            SCOREP_USER_REGION("memory_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            auto& my_mem_buffer = roco2::thread_local_memory().mem_buffer;

            constexpr std::size_t chunksize = ChunkSize / sizeof(my_mem_buffer[0]);

            static_assert(ChunkSize % sizeof(my_mem_buffer[0]) == 0,
                          "Given ChunkSize paramter should be divisible by sizeof(uint64_t)");

            static_assert(chunksize <= roco2::detail::thread_local_memory::mem_size,
                          "Given ChunkSize parameter is to big.");

            uint64_t m = 0;
            std::size_t loops = 0;
            do
            {
                // SCOREP_USER_REGION("memory_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)
                for (std::size_t i = 0; i < chunksize; i++)
                {
                    m += my_mem_buffer[i];
                }

                loops++;
            } while (cond());

            roco2::metrics::utility::instance().write(loops);

            // just as a data dependency
            volatile int dd = 0;
            if (m == 42)
                dd++;
        }
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_MEMORY_READ_HPP
