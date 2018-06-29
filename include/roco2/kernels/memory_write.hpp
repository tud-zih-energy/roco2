#ifndef INCLUDE_ROCO2_KERNELS_MEMORY_WRITE_HPP
#define INCLUDE_ROCO2_KERNELS_MEMORY_WRITE_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
// #include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{

    template <std::size_t ChunkSize = 64 * 1024 * 1024>
    class memory_write : public base_kernel
    {
    public:
        virtual experiment_tag tag() const override
        {
            return 13;
        }

    private:
        virtual void run_kernel(chrono::time_point until) override
        {
            // SCOREP_USER_REGION("memory_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            auto& my_mem_buffer = roco2::thread_local_memory().mem_buffer;

            constexpr std::size_t chunksize = ChunkSize / sizeof(my_mem_buffer[0]);

            static_assert(ChunkSize % sizeof(my_mem_buffer[0]) == 0,
                          "Given ChunkSize paramter should be divisible by sizeof(uint64_t)");

            static_assert(chunksize <= roco2::detail::thread_local_memory::mem_size,
                          "Given ChunkSize parameter is to big.");

            std::size_t loops = 0;
            do
            {
                // SCOREP_USER_REGION("memory_kernel_loop", SCOREP_USER_REGION_TYPE_FUNCTION)
                for (std::size_t i = 0; i < chunksize; i++)
                {
                    my_mem_buffer[i] = loops;
                }

                loops++;
            } while (std::chrono::high_resolution_clock::now() < until);

            roco2::metrics::utility::instance().write(loops);
        }
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_MEMORY_WRITE_HPP
