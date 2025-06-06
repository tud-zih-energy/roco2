#pragma once

#include <roco2/kernels/base_gpu_kernel.hpp>

namespace roco2
{
namespace kernels
{

    class naive_mm_gpu_kernel : public base_gpu_kernel
    {
    public:
        experiment_tag_t tag() const override
        {
            return 100;
        }

        void run_kernel(int on_gpu) override;

        std::vector<void*> d_A_;
        std::vector<void*> d_B_;
        std::vector<void*> d_C_;

        dim3 gridDim;
        dim3 blockDim;
    };
} // namespace kernels
} // namespace roco2
