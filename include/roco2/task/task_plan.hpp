#ifndef INCLUDE_ROCO2_TASK_TASK_PLAN_HPP
#define INCLUDE_ROCO2_TASK_TASK_PLAN_HPP

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>
#include <roco2/task/task.hpp>

#include <memory>
#include <vector>

namespace roco2
{
namespace task
{
    class task_plan
    {
    public:
        roco2::chrono::duration eta() const
        {
            return eta_;
        }

        void push_back(std::unique_ptr<task>&& t)
        {
            eta_ += t->eta();
            tasks_.push_back(std::move(t));
        }

        template <typename Task>
        void push_back(Task&& t)
        {
            push_back(static_cast<std::unique_ptr<task>>(std::make_unique<Task>(t)));
        }

        void execute()
        {
            for (auto& task : tasks_)
            {
#pragma omp master
                {
                    log::info() << "ETA: "
                                << std::chrono::duration_cast<std::chrono::seconds>(eta_);
                }

                task->execute();
                eta_ -= task->eta();
            }

            executed_ = true;
        }

    private:
        bool executed_ = false;
        std::vector<std::unique_ptr<task>> tasks_;
        roco2::chrono::duration eta_ = roco2::chrono::duration(0);
    };
}
}

#endif // INCLUDE_ROCO2_TASK_TASK_HPP
