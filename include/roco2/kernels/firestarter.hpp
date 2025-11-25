#ifndef INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
#define INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP

#include <roco2/chrono/util.hpp>
#include <roco2/kernels/base_kernel.hpp>

#include <firestarter/Payload/CompiledPayload.hpp>
#include <firestarter/Payload/Payload.hpp>

namespace roco2
{
namespace kernels
{

    class firestarter : public base_kernel
    {

        using param_type = uint64_t;

    public:
        firestarter();

        virtual experiment_tag tag() const override
        {
            return 6;
        }

    private:
        /// Runs the firestarter kernel until a timepoint
        /// \arg until The timepoint until the kernel should execute the high load function of
        /// firestarter
        void run_kernel(roco2::chrono::time_point until) override;

        /// The number of hot loop iterations
        const static param_type loop_count = 10000;

        /// The unique ptr to the load and init function for firestarter
        ::firestarter::payload::CompiledPayload::UniquePtr compiled_payload_ptr = { nullptr,
                                                                                    nullptr };
    };
} // namespace kernels
} // namespace roco2

#endif // INCLUDE_ROCO2_KERNELS_FIRESTARTER_HPP
