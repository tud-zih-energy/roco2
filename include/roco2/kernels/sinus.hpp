#ifndef INCLUDE_ROCO2_KERNELS_SINUS_HPP
#define INCLUDE_ROCO2_KERNELS_SINUS_HPP

#include <roco2/kernels/base_kernel.hpp>
#include <roco2/memory/thread_local.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

#include <cmath>

namespace roco2
{
namespace kernels
{
    template <typename return_functor>
    class sinus : public base_kernel<return_functor>
    {
    public:
        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 10;
        }

    private:
        virtual void run_kernel(return_functor& cond) override
        {
            SCOREP_USER_REGION("sinus_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)

            double m = 0.0;

            std::size_t loops = 0;

            do
            {
                for (std::size_t i = 0; i < sinus_loop; i++)
                {
                    m += sin((double)i);
                }

                loops++;
            } while (cond());

            roco2::metrics::utility::instance().write(loops);

            // just as a data dependency
            volatile int dd = 0;
            if (m == 42.0)
                dd++;
        }

        static const std::size_t sinus_loop = 200000;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_SINUS_HPP
