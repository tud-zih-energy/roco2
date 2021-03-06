#ifndef INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP
#define INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP

#include <roco2/chrono/chrono.hpp>
#include <roco2/experiments/cpu_sets/cpu_set.hpp>
#include <roco2/kernels/base_kernel.hpp>

#include <chrono>

namespace roco2
{

namespace experiments
{

    class base
    {
    public:
        using time_point = std::chrono::high_resolution_clock::time_point;
        using duration = time_point::duration;

        base(time_point starting_point) : starting_point(starting_point)
        {
        }

        virtual void run(roco2::kernels::base_kernel& kernel,
                         roco2::experiments::cpu_sets::cpu_set on) = 0;

        virtual roco2::chrono::duration eta() const = 0;

    protected:
        void run_for(roco2::kernels::base_kernel& kernel, roco2::experiments::cpu_sets::cpu_set on,
                     duration length)
        {
            starting_point += length;

            kernel.run(starting_point, on);
        }

    private:
        time_point starting_point;
    };
}
}

#endif // INCLUDE_ROCO2_EXPERIMENTS_BASE_HPP
