#ifndef INCLUDE_ROCO2_KERNELS_HIGH_LOW_HPP
#define INCLUDE_ROCO2_KERNELS_HIGH_LOW_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/kernels/base_kernel.hpp>
#include <roco2/metrics/utility.hpp>
#include <roco2/scorep.hpp>

namespace roco2
{
namespace kernels
{
    template <typename return_functor>
    class high_low_bs : public base_kernel<return_functor>
    {
    public:
        template <class DT_HIGH, class DT_LOW>
        high_low_bs(DT_HIGH high_time, DT_LOW low_time)
        : high_time_(std::chrono::duration_cast<roco2::chrono::time_point::duration>(high_time)),
          low_time_(std::chrono::duration_cast<roco2::chrono::time_point::duration>(low_time))
        {
        }

        virtual typename base_kernel<return_functor>::experiment_tag tag() const override
        {
            return 12;
        }

    private:
        void run_kernel(return_functor& cond()) override
        {
            SCOREP_USER_REGION("high_low_bs_kernel", SCOREP_USER_REGION_TYPE_FUNCTION)
            roco2::chrono::time_point deadline = std::chrono::high_resolution_clock::now();

            std::size_t loops = 0;

            while (cond())
            {
                deadline += high_time_;
                roco2::chrono::busy_wait_until(deadline);
                deadline += low_time_;
                std::this_thread::sleep_until(deadline);

                loops++;
            }

            roco2::metrics::utility::instance().write(loops);
        }

        roco2::chrono::time_point::duration high_time_;
        roco2::chrono::time_point::duration low_time_;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_HIGH_LOW_HPP
