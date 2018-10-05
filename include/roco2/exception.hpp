#ifndef INCLUDE_ROCO2_EXCEPTION_HPP
#define INCLUDE_ROCO2_EXCEPTION_HPP

#include <roco2/log.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

namespace roco2
{
struct exception : std::runtime_error
{
    explicit exception(const std::string& arg) : std::runtime_error(arg)
    {
        log::error() << arg;
    }
};

namespace detail
{

    template <typename Arg, typename... Args>
    class make_exception
    {
    public:
        void operator()(std::stringstream& msg, Arg arg, Args... args)
        {
            msg << arg;
            make_exception<Args...>()(msg, args...);
        }
    };

    template <typename Arg>
    class make_exception<Arg>
    {
    public:
        void operator()(std::stringstream& msg, Arg arg)
        {
            msg << arg;
        }
    };
}

template <typename... Args>
[[noreturn]] inline void raise(Args... args)
{
    std::stringstream msg;

    detail::make_exception<Args...>()(msg, args...);

    throw exception(msg.str());
}
} // namespace roco2

#endif // INCLUDE_ROCO2_EXCEPTION_HPP
