#ifndef INCLUDE_ROCO2_TASK_LAMBDA_TASK_HPP
#define INCLUDE_ROCO2_TASK_LAMBDA_TASK_HPP

#include <roco2/task/task.hpp>

namespace roco2
{
namespace task
{
    template <typename F>
    class lambda_task : public task
    {
    public:
        lambda_task(F f) : f(f)
        {
        }

        virtual roco2::chrono::duration eta() const override
        {
            return roco2::chrono::duration(0);
        }

        virtual void execute() override
        {
            f();
        }

    private:
        F f;
    };

    template <typename F>
    lambda_task<F> make_lambda_task(F f)
    {
        return lambda_task<F>(f);
    }
}
}

#endif // INCLUDE_ROCO2_TASK_LAMBDA_TASK_HPP
