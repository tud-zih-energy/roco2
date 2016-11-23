#ifndef INCLUDE_ROCO2_MEMORY_NUMA_HPP
#define INCLUDE_ROCO2_MEMORY_NUMA_HPP

#include <roco2/cpu/info.hpp>

#include <roco2/log.hpp>

#include <numa.h>

namespace roco2
{
namespace memory
{

    class numa_bind_local
    {

        class numa_bitmask
        {
        public:
            numa_bitmask()
            {
                mask_ = numa_bitmask_alloc((unsigned int)numa_max_possible_node());
            }

            explicit numa_bitmask(bitmask* p) : mask_(p)
            {
            }

            ~numa_bitmask()
            {
                numa_bitmask_free(mask_);
            }

            numa_bitmask(const numa_bitmask&) = delete;
            numa_bitmask& operator=(const numa_bitmask&) = delete;

            numa_bitmask(numa_bitmask&&) = default;
            numa_bitmask& operator=(numa_bitmask&&) = default;

            operator bitmask*()
            {
                return mask_;
            }

        public:
            void set_all()
            {
                numa_bitmask_setall(mask_);
            }

            void clear_all()
            {
                numa_bitmask_clearall(mask_);
            }

            void set_bit(unsigned int bit)
            {
                numa_bitmask_setbit(mask_, bit);
            }

            friend bool operator==(const numa_bitmask& a, const numa_bitmask& b)
            {
                return numa_bitmask_equal(a.mask_, b.mask_);
            }

            friend bool operator!=(const numa_bitmask& a, const numa_bitmask& b)
            {
                return !(a == b);
            }

        private:
            bitmask* mask_;
        };

    public:
        numa_bind_local()
        {
            int numa_node = numa_node_of_cpu(roco2::cpu::info::current_cpu());

            if (numa_node == -1)
            {
                log::warn() << "Couldn't bound on local numa node";

                return;
            }

            numa_bitmask mask;
            mask.clear_all();
            mask.set_bit(static_cast<unsigned int>(numa_node));
            numa_set_membind(mask);

            numa_bitmask check;
            numa_get_bitmask(check);

            if (mask != check)
            {
                log::warn() << "Couldn't bound on local numa node";

                return;
            }

            log::info() << "Bound on local numa node";
        }

        ~numa_bind_local()
        {
            numa_bitmask mask;

            mask.set_all();

            numa_set_membind(mask);
        }
    };
}
}

#endif // INCLUDE_ROCO2_MEMORY_NUMA_HPP
