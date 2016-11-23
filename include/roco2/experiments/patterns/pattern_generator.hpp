#ifndef INCLUDE_ROCO2_PATTERNS_PATTERN_GENERATOR_HPP
#define INCLUDE_ROCO2_PATTERNS_PATTERN_GENERATOR_HPP

#include <roco2/experiments/cpu_sets/cpu_set_generator.hpp>
#include <roco2/experiments/patterns/pattern.hpp>

#include <iostream>
#include <vector>

namespace roco2
{
namespace experiments
{

    namespace patterns
    {

        enum class triangle_shape
        {
            none,
            upper,
            lower
        };

        inline pattern block_pattern(std::size_t block_size, bool reverse = false,
                                     triangle_shape shape = triangle_shape::none)
        {
            if (omp_get_num_threads() % block_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into blocks of size "
                                   << block_size;
            }

            auto num_blocks = omp_get_num_threads() / block_size;

            pattern result;

            // Fun fact: I've seen this pearl of overly clever code, but I couldn't change it.
            //           Future me, have fun proving once again, this is correct ¯\_(ツ)_/¯
            std::size_t diff = reverse ? -1 : 1;
            for (std::size_t nth = reverse ? num_blocks - 1 : 0; nth < num_blocks; nth += diff)
            {
                auto start = nth * block_size;

                switch (shape)
                {
                case triangle_shape::none:
                    result.append(cpu_sets::make_cpu_range(start, start + block_size - 1));
                    break;
                case triangle_shape::upper:
                    result.append(cpu_sets::make_cpu_range(0, start + block_size - 1));
                    break;

                case triangle_shape::lower:
                    result.append(cpu_sets::make_cpu_range(start, omp_get_num_threads() - 1));
                    break;
                }
            }

            return result;
        }

        inline pattern sub_block_on(uint32_t socket, std::size_t block_size = 4)
        {
            auto cores = roco2::cpu::topology::instance().get_socket(socket).cores;
            auto num_cores = cores.size();

            pattern result;

            cpu_sets::cpu_set range;

            for (std::size_t i = 0; i < num_cores; i += block_size)
            {
                for (std::size_t j = i; j < i + block_size; ++j)
                    range.add(cores[j]);

                result.append(range);
            }

            return result;
        }

        inline pattern sub_block_on_all(std::size_t block_size = 4)
        {
            pattern result;

            for (auto& socket : roco2::cpu::topology::instance().sockets())
            {
                result = result >> sub_block_on(socket.id, block_size);
            }

            return result;
        }

        inline pattern block_on_all()
        {
            auto num_cores = roco2::cpu::topology::instance().num_per_socket();

            std::size_t block_size = 4;

            while (num_cores % block_size != 0)
                --block_size;

            return block_pattern(block_size, false, triangle_shape::upper);
        }

        inline pattern sub_block_pattern(std::size_t block_size, std::size_t stride_size)
        {
            if (omp_get_num_threads() % block_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into blocks of size "
                                   << block_size;
            }

            if (omp_get_num_threads() % stride_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into strides of size "
                                   << stride_size;
            }

            auto num_strides = omp_get_num_threads() / stride_size;

            if (stride_size % block_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into stride blocks of size "
                                   << block_size;
            }

            // blocks per stride
            auto num_blocks = stride_size / block_size;

            pattern result;

            for (std::size_t nth_stride = 0; nth_stride < num_strides; nth_stride++)
            {
                cpu_sets::cpu_set range;

                for (std::size_t nth = 0; nth < num_blocks; ++nth)
                {
                    auto start = nth_stride * stride_size + nth * block_size;

                    range = range | cpu_sets::make_cpu_range(start, start + block_size - 1);

                    result.append(range);
                }
            }

            return result;
        }

        inline pattern stride_pattern(std::size_t block_size, std::size_t stride_size)
        {
            if (omp_get_num_threads() % block_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into blocks of size "
                                   << block_size;
            }

            if (omp_get_num_threads() % stride_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into strides of size "
                                   << stride_size;
            }

            auto num_strides = omp_get_num_threads() / stride_size;

            if (stride_size % block_size != 0)
            {
                roco2::log::warn() << omp_get_num_threads()
                                   << " threads cannot be divided into stride blocks of size "
                                   << block_size;
            }

            auto num_experiments = stride_size / block_size;

            pattern result;

            cpu_sets::cpu_set range;

            for (std::size_t nth = 0; nth < num_experiments; ++nth)
            {
                for (std::size_t nth_stride = 0; nth_stride < num_strides; nth_stride++)
                {
                    auto start = nth * block_size + nth_stride * stride_size;

                    range = range | cpu_sets::make_cpu_range(start, start + block_size - 1);
                }

                result.append(range);
            }

            return result;
        }
    }
}
}

#endif // INCLUDE_ROCO2_PATTERNS_PATTERN_GENERATOR_HPP
