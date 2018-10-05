#pragma once

#include <roco2/cpu/info.hpp>
#include <roco2/exception.hpp>
#include <roco2/log.hpp>
#include <roco2/metrics/shell.hpp>

#include <cstdlib>
#include <tuple>

namespace roco2
{

namespace cpu
{
    class shell
    {
    public:
        using setting_type = std::pair<std::int64_t, std::string>;

        shell(const std::string& prefix, const std::string& initialize, const std::string& finalize) : prefix(prefix), initialize(initialize), finalize(finalize)
        {
            if (!initialize.empty())
            {
                 call(initialize);
            }
#pragma omp barrier
            roco2::metrics::shell::instance().write(-1);
        }

        ~shell()
        {
            if (!finalize.empty())
            {
                 call(finalize);
            }
#pragma omp barrier
            roco2::metrics::shell::instance().write(-1);
        }

        shell(const shell&) = delete;
        shell& operator=(const shell&) = delete;

        void change(const setting_type& setting)
        {
            roco2::metrics::shell::instance().write(setting.first);
            call(prefix + setting.second);
        }

    private:
        void call(const std::string& cmd)
        {
#pragma omp barrier
#pragma omp master
            {
                auto ret = std::system(cmd.c_str());
                if (ret)
                {
                    log::warn() << "shell command failed (" << ret << ") " << cmd;
                }
                else
                {
                    log::info() << "shell command successful: " << cmd;
                }
            }
#pragma omp barrier
        }

    private:
        std::string prefix;
        std::string initialize;
        std::string finalize;
    };
}
}
