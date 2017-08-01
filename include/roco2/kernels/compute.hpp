#ifndef INCLUDE_ROCO2_KERNELS_COMPUTE_HPP
#define INCLUDE_ROCO2_KERNELS_COMPUTE_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{
    template <typename return_functor>
    class compute : public base_kernel<return_functor>
    {
    public:
        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 5;
        }

    private:
        template <typename Container>
        void compute_kernel(Container& A, Container& B, Container& C, std::size_t repeat)
        {
            double m = C[0];
            const auto size = thread_local_memory().vec_size;

            for (std::size_t i = 0; i < repeat; i++)
            {
                for (uint64_t i = 0; i < size; i++)
                {
                    m += B[i] * A[i];
                }
                C[0] = m;
            }
        }

        virtual void run_kernel(return_functor& cond) override
        {
            SCOREP_USER_REGION("compute_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            auto& vec_A = roco2::thread_local_memory().vec_A;
            auto& vec_B = roco2::thread_local_memory().vec_B;
            auto& vec_C = roco2::thread_local_memory().vec_C;

            std::size_t loops = 0;

            while (cond())
            {
                if (vec_C[0] == 123.12345)
                    vec_A[0] += 1.0;

                compute_kernel(vec_A, vec_B, vec_C, 32);

                loops++;
            }

            roco2::metrics::utility::instance().write(loops);

            // just as a data dependency
            volatile int dd = 0;
            if (vec_C[0] == 42.0)
                dd++;
        }
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_COMPUTE_HPP
