#pragma once

#include <roco2/kernels/base_gpu_kernel.hpp>

namespace roco2
{
namespace kernels
{

    class naive_mm_gpu_kernel : public base_gpu_kernel
    {
    public:
        naive_mm_gpu_kernel();
        virtual ~naive_mm_gpu_kernel();

        experiment_tag_t tag() const override
        {
            return 100;
        }

        void run_kernel(int on_gpu) override;

    private:
        std::vector<double*> d_A_;
        std::vector<double*> d_B_;
        std::vector<double*> d_C_;

        int N;
    };
} // namespace kernels
} // namespace roco2
