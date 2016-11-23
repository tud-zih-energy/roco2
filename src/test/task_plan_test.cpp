#include <roco2/log.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

int main()
{

#pragma omp parallel
    {
        roco2::task::task_plan plan;

        for (int i = 0; i < 10; i++)
            plan.push_back(roco2::task::make_lambda_task(
                [&]() { roco2::chrono::busy_wait_for(std::chrono::seconds(1)); }));

#pragma omp barrier
        roco2::log::info() << plan.eta();
    }

    return 0;
}
