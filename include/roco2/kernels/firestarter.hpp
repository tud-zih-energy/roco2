#ifndef INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
#define INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP

#include <roco2/kernels/base_kernel.hpp>

#include <roco2/chrono/util.hpp>

#include <memory>

namespace roco2
{
namespace kernels
{
    using firestarter_function = int (*)(void*);

    firestarter_function init_firestarter_kernel();

    void* firestartet_data(unsigned long long loop_count,
                           std::vector<double, AlignmentAllocator<double, 32>>& my_mem_buffer);

    void* free_firestartet_data(void* data);

    template <typename return_functor>
    class firestarter : public base_kernel<return_functor>
    {

    public:
        firestarter() : firestarter_function_(init_firestarter_kernel())
        {
        }

        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 6;
        }

    private:
        void run_kernel(return_functor& cond) override
        {
            SCOREP_USER_REGION("firestarter_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            assert(firestarter_function_);

            auto& my_mem_buffer = roco2::thread_local_memory().firestarter_buffer;

            // check alignment requirements
            assert(reinterpret_cast<unsigned long long>(my_mem_buffer.data()) % 32 == 0);

            auto data = firestartet_data(loop_count, my_mem_buffer);

            std::size_t loops = 0;

            do
            {
                // SCOREP_USER_REGION("firestarter_kernel_loop",
                // SCOREP_USER_REGION_TYPE_FUNCTION)
                firestarter_function_(data);

                loops++;
            } while (cond());

            roco2::metrics::utility::instance().write(loops);

            free_firestartet_data(data);
        }

        firestarter_function firestarter_function_;

        const static unsigned long long loop_count = 10000;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
