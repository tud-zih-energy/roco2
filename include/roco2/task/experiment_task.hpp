#ifndef INCLUDE_ROCO2_TASK_EXPERIMENT_TASK_HPP
#define INCLUDE_ROCO2_TASK_EXPERIMENT_TASK_HPP

#include <roco2/task/task.hpp>

#include <roco2/experiments/base.hpp>

namespace roco2
{
namespace task
{
    template <typename return_functor>
    class experiment_task : public task
    {
    public:
        experiment_task(roco2::experiments::base<return_functor>& exp,
                        roco2::kernels::base_kernel<return_functor>& kernel,
                        const roco2::experiments::cpu_sets::cpu_set& on)
        : exp_(exp), kernel_(kernel), on_(on)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return exp_.eta();
        }

        virtual void execute() override
        {
            exp_.run(kernel_, on_);
        }

    private:
        roco2::experiments::base<return_functor>& exp_;
        roco2::kernels::base_kernel<return_functor>& kernel_;
        roco2::experiments::cpu_sets::cpu_set on_;
    };
}
}

#endif // INCLUDE_ROCO2_TASK_EXPERIMENT_TASK_HPP
