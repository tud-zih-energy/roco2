#ifndef INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP

#include <roco2/experiments/base.hpp>

namespace roco2
{

namespace experiments
{

    class const_lenght : public base
    {
    public:
        using base::time_point;
        using base::duration;

        const_lenght(time_point starting_point, duration length)
        : base(starting_point), length(length)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return length;
        }

        virtual void run(roco2::kernels::base_kernel& kernel,
                         roco2::experiments::cpu_sets::cpu_set on) override
        {
            this->run_for(kernel, on, length);
        }

    private:
        duration length;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
