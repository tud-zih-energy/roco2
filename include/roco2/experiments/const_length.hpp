#ifndef INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP

#include <roco2/experiments/base.hpp>

namespace roco2
{

namespace experiments
{
    template <typename return_functor>
    class const_lenght : public base<return_functor>
    {
    public:
        using typename base<return_functor>::time_point;

        const_lenght(time_point starting_point, return_functor& cond)
        : base<return_functor>(starting_point), cond(cond)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return cond.eta();
        }

        virtual void run(roco2::kernels::base_kernel<return_functor>& kernel,
                         roco2::experiments::cpu_sets::cpu_set on) override
        {
            this->run_for(kernel, on, cond);
            cond.next();
        }

    private:
        return_functor cond;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
