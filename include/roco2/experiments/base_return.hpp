/*
 * base_return.hpp
 *
 *  Created on: 03.08.2017
 *      Author: gocht
 */

#ifndef INCLUDE_ROCO2_EXPERIMENTS_BASE_RETURN_HPP_
#define INCLUDE_ROCO2_EXPERIMENTS_BASE_RETURN_HPP_

#include <roco2/chrono/chrono.hpp>

namespace roco2
{
namespace experiments
{
    class base_return
    {
    public:
        virtual void sync_working() = 0;
        virtual void sync_idle() = 0;
        virtual roco2::chrono::duration eta() const = 0;
        virtual void next() = 0;
        virtual bool operator()() = 0;
    };
}
}

#endif /* INCLUDE_ROCO2_EXPERIMENTS_BASE_RETURN_HPP_ */
