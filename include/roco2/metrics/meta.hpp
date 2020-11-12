#pragma once

#include <cstdint>

namespace roco2
{
namespace metrics
{
    class meta
    {
        meta()
        {
        }

    public:
        meta(const meta&) = delete;
        meta& operator=(const meta&) = delete;

        static meta& instance()
        {
            // gcc doesn't like omp threadprivate classes
            static thread_local meta e;
            return e;
        }

        void trigger()
        {
            write(experiment, frequency, shell, threads, op1, op2);
        }

        void write(std::uint64_t experiment, std::uint64_t frequency, std::int64_t shell,
                   std::uint64_t threads, std::uint64_t op1, std::uint64_t op2);

        std::uint64_t experiment;
        std::uint64_t frequency;
        std::int64_t shell;
        std::uint64_t threads;
        std::uint64_t op1;
        std::uint64_t op2;
    };
} // namespace metrics
} // namespace roco2
