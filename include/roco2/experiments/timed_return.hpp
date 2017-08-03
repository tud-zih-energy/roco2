/*
 * timed_return.hpp
 *
 *  Created on: 03.08.2017
 *      Author: gocht
 */

#ifndef INCLUDE_ROCO2_EXPERIMENTS_TIMED_RETURN_HPP_
#define INCLUDE_ROCO2_EXPERIMENTS_TIMED_RETURN_HPP_

#include <roco2/experiments/base_return.hpp>

#include <thread>

namespace roco2
{
namespace experiments
{

    class timed_return : public base_return
    {

    public:
        timed_return(roco2::chrono::duration d)
        : base_return(), until(std::chrono::high_resolution_clock::now() + d), d(d)
        {
        }

        roco2::chrono::duration eta() const override
        {
            return d;
        }

        void sync_working() override
        {
        }

        void sync_idle() override
        {
            std::this_thread::sleep_until(until);
        }

        void next() override
        {
            until = std::chrono::high_resolution_clock::now() + d;
        }

        bool operator()() override
        {
            return std::chrono::high_resolution_clock::now() < until;
        }

    private:
        roco2::chrono::time_point until;
        roco2::chrono::duration d;
    };
}
}

#endif /* INCLUDE_ROCO2_EXPERIMENTS_TIMED_RETURN_HPP_ */
