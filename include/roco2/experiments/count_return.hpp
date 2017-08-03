/*
 * timed_return.hpp
 *
 *  Created on: 03.08.2017
 *      Author: gocht
 */

#ifndef INCLUDE_ROCO2_EXPERIMENTS_COUNT_RETURN_HPP_
#define INCLUDE_ROCO2_EXPERIMENTS_COUNT_RETURN_HPP_

#include <chrono>
#include <roco2/chrono/chrono.hpp>
#include <roco2/cpu/topology.hpp>
#include <roco2/experiments/base_return.hpp>

#include <atomic>
#include <condition_variable>
#include <thread>

namespace roco2
{
namespace experiments
{

    static int working_threads = 0;
    static int waiting_threads = 0;
    static std::condition_variable cv;
    static std::mutex cv_m;

    class count_return : public base_return
    {

    public:
        count_return(int iterations) : base_return(), iterations(iterations), current_count(0)
        {
            std::unique_lock<std::mutex> lk(cv_m);
            if (working_threads == 0)
            {
                working_threads = roco2::cpu::topology::instance().num_cores();
            }
            log::debug() << "there are " << working_threads << " ";
        }

        roco2::chrono::duration eta() const override
        {
            return std::chrono::milliseconds(10);
        }

        void sync_working() override
        {
            std::unique_lock<std::mutex> lk(cv_m);
            working_threads--;
            log::debug() << "working: there are sill " << working_threads << " threads waiting";
            if (working_threads > 0)
            {
                cv.wait(lk, [] { return working_threads == 0; });
            }
            else
            {
                log::debug() << "working notify";
                cv.notify_all();
            }
            log::debug() << "working: proceed";
        }

        void sync_idle() override
        {
            std::unique_lock<std::mutex> lk(cv_m);
            working_threads--;
            log::debug() << "idle: there are sill " << working_threads << " threads waiting";
            if (working_threads > 0)
            {
                cv.wait(lk, [] { return working_threads == 0; });
            }
            else
            {
                log::debug() << "idle notify";
                cv.notify_all();
            }
            log::debug() << "idle: proceed";
        }

        void next() override
        {
            std::unique_lock<std::mutex> lk(cv_m);
            current_count = 0;

            waiting_threads++;

            if (waiting_threads == roco2::cpu::topology::instance().num_cores())
            {
                log::debug() << "set working threads";
                working_threads = roco2::cpu::topology::instance().num_cores();
                waiting_threads = 0;
                cv.notify_all();
            }
            else
            {
                cv.wait(lk, [] { return waiting_threads == 0; });
            }
        }

        bool operator()() override
        {
            current_count++;
            return current_count < iterations;
        }

    private:
        int iterations;
        int current_count;
    };
}
}

#endif /* INCLUDE_ROCO2_EXPERIMENTS_COUNT_RETURN_HPP_ */
