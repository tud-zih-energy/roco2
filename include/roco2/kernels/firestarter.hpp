#ifndef INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
#define INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP

#include <roco2/kernels/base_kernel.hpp>

#include <roco2/chrono/util.hpp>

namespace roco2
{
namespace kernels
{

    class firestarter : public base_kernel
    {

        using param_type = unsigned long long;

    public:
        firestarter();

        virtual experiment_tag tag() const override
        {
            return 6;
        }

    private:
        void run_kernel(roco2::chrono::time_point until) override;

        int (*firestarter_function_)(void*);
        int (*firestarter_init_)(void*);
        int base_function_;

        const static param_type loop_count = 10000;
    };
}
}

#endif // INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
