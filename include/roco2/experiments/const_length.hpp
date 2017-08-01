#ifndef INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP

#include <roco2/experiments/base.hpp>
#include <roco2/experiments/timed_return.hpp>

namespace roco2
{

namespace experiments
{
    class const_lenght : public base<timed_return>
    {
    public:
        using base<timed_return>::time_point;
        using base<timed_return>::duration;

        const_lenght(time_point starting_point, duration d)
        : base<timed_return>(starting_point), cond(d)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return cond.eta();
        }

        virtual void run(roco2::kernels::base_kernel<timed_return>& kernel,
                         roco2::experiments::cpu_sets::cpu_set on) override
        {
            this->run_for(kernel, on, cond);
            cond.next();
        }

    private:
        timed_return cond;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_CONST_LENGTH_HPP
