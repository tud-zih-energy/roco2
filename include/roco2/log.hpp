#ifndef INCLUDE_ROCO2_LOG_HPP
#define INCLUDE_ROCO2_LOG_HPP

#define NITRO_LOG_MIN_SEVERITY debug

#include <nitro/log/log.hpp>

#include <nitro/log/attribute/message.hpp>
#include <nitro/log/attribute/omp_thread_id.hpp>
#include <nitro/log/attribute/severity.hpp>
#include <nitro/log/attribute/timestamp.hpp>

#include <nitro/log/sink/stdout_omp.hpp>

#include <nitro/log/filter/severity_filter.hpp>

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace roco2
{

namespace logging
{
    using record =
        nitro::log::record<nitro::log::severity_attribute, nitro::log::message_attribute,
                           nitro::log::omp_thread_id_attribute, nitro::log::timestamp_attribute>;

    template <typename Record>
    class formatter
    {
    public:
        int width() const
        {
            auto max = omp_get_max_threads();

            if (max >= 1000)
                return 4;

            if (max >= 100)
                return 3;

            if (max >= 10)
                return 2;

            return 1;
        }

        std::string format(Record& r)
        {
            std::stringstream s;
            s << "[" << r.timestamp().count() << "][Thread: " << std::setw(width())
              << r.omp_thread_id() << "][" << r.severity() << "]: " << r.message();

            return s.str();
        }
    };

    template <typename Record>
    using filter = nitro::log::filter::severity_filter<Record>;
}

using log = nitro::log::logger<logging::record, logging::formatter, nitro::log::sink::stdout_omp,
                               logging::filter>;
}

#endif // INCLUDE_ROCO2_LOG_HPP
