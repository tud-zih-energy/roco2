#ifndef INCLUDE_ROCO2_ENVIRONMENT_HPP
#define INCLUDE_ROCO2_ENVIRONMENT_HPP

#include <string>
#include <cstdlib>

#include <roco2/omp_lock_guard.hpp>

namespace roco2
{
namespace enviroment
{
    /**
     * \brief returns the given ENV variables
     *
     * Returns the value of the given enviroment variable.
     * Returns an empty string, if the variable isn't set.
     */
    std::string get_variable(std::string name)
    {
        static omp_lock lock;

        omp_guard guard(lock);

        char* tmp;

        tmp = std::getenv(name.c_str());

        if (tmp == nullptr)
            return std::string("");

        return std::string(tmp);
    }
}
}

#endif // INCLUDE_ROCO2_ENVIRONMENT_HPP