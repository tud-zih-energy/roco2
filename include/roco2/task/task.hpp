#ifndef INCLUDE_ROCO2_TASK_TASK_HPP
#define INCLUDE_ROCO2_TASK_TASK_HPP

#include <roco2/chrono/chrono.hpp>

namespace roco2
{
namespace task
{
    class task
    {
    public:
        virtual roco2::chrono::duration eta() const = 0;
        virtual void execute() = 0;

        virtual ~task()
        {
        }
    };
}
}

#endif // INCLUDE_ROCO2_TASK_TASK_HPP
