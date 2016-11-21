#ifndef INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP

#include <roco2/experiments/base.hpp>
#include <roco2/log.hpp>

namespace roco2
{

namespace experiments
{

    class shrink_lenght : public base
    {
    public:
        using base::time_point;
        using base::duration;

        shrink_lenght(time_point starting_point, duration starting_length, duration min_length,
                      double factor)
        : base(starting_point), length(starting_length), min_length(min_length), factor(factor)
        {
            log::warn() << "Using shrink_lenght experiments. ETA might be not correct at all.";
        }

        virtual roco2::chrono::duration eta() const override
        {
            if (length > min_length)
            {
                return std::max(length * factor, min_length);
            }
            else
            {
                return lenght;
            }
        }

        virtual void run(kernels::base_kernel& kernel, ranges::cpu_range on) override
        {
            if (length > min_length)
            {
                length = std::max(length * factor, min_length);
            }

            this->run_for(kernel, on, length);
        }

    private:
        duration length;
        duration min_length;
        double factor;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_SHRINK_LENGTH_HPP
