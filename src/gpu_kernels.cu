#include <roco2/kernels/naive_mm_gpu_kernel.hpp>
#include <roco2/metrics/utility.hpp>

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include <chrono>
#include <iostream>

__global__ void matrixMulNaive(const double* A, const double* B, double* C, int N)
{
    for (int row = blockIdx.y * blockDim.y + threadIdx.y; row < N; row += gridDim.y * blockDim.y)
    {
        for (int col = blockIdx.x * blockDim.x + threadIdx.x; col < N;
             col += gridDim.x * blockDim.x)
        {

            double sum = 0.0;

            for (int k = 0; k < N; ++k)
            {
                sum += A[row * N + k] * B[k * N + col];
            }

            // TODO this would require a += to be correct
            C[row * N + col] = sum;
        }
    }
}

namespace roco2
{
namespace kernels
{
    naive_mm_gpu_kernel::naive_mm_gpu_kernel()
    {
        int num_gpus;
        cudaGetDeviceCount(&num_gpus);

        double* h_A = new double[N * N];
        double* h_B = new double[N * N];
        double* h_C = new double[N * N];

        for (int row = 0; row < N; ++row)
        {
            for (int col = 0; col < N; ++col)
            {
                h_A[row * N + col] = row;
                h_B[row * N + col] = col + 2;
                h_C[row * N + col] = 0;
            }
        }

        const int NUM_BYTES = N * N * sizeof(double);

        for (int gpu_id = 0; gpu_id < num_gpus; gpu_id++)
        {
            cudaSetDevice(gpu_id);

            d_A_.push_back(nullptr);
            d_B_.push_back(nullptr);
            d_C_.push_back(nullptr);

            cudaMalloc(&d_A_[gpu_id], NUM_BYTES);
            cudaMalloc(&d_B_[gpu_id], NUM_BYTES);
            cudaMalloc(&d_C_[gpu_id], NUM_BYTES);

            cudaMemcpy(d_A_[gpu_id], h_A, NUM_BYTES, cudaMemcpyHostToDevice);
            cudaMemcpy(d_B_[gpu_id], h_B, NUM_BYTES, cudaMemcpyHostToDevice);
            cudaMemcpy(d_C_[gpu_id], h_C, NUM_BYTES, cudaMemcpyHostToDevice);
        }

        delete[] h_A;
        delete[] h_B;
        delete[] h_C;

        int numberOfSMs;
        cuDeviceGetAttribute(&numberOfSMs, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, 0);
        int sizeOfWarps = 32;

        N = numberOfSMs * sizeOfWarps;

        gridDim = dim3(numberOfSMs, numberOfSMs);
        blockDim = dim3(sizeOfWarps / 2, sizeOfWarps / 2);
    }

    naive_mm_gpu_kernel::~naive_mm_gpu_kernel()
    {
        int num_gpus;
        cudaGetDeviceCount(&num_gpus);

        assert(d_A_.size() == num_gpus);

        for (int gpu_id = 0; gpu_id < num_gpus; gpu_id++)
        {
            cudaSetDevice(gpu_id);

            cudaFree(d_A_[gpu_id]);
            cudaFree(d_B_[gpu_id]);
            cudaFree(d_C_[gpu_id]);
        }
    }

    void naive_mm_gpu_kernel::run_kernel(int gpu_id)
    {
        cudaSetDevice(gpu_id);

        std::size_t loops = 0;

        const auto REPEATS = 100;

        while (running_)
        {
            for (int i = 0; i < REPEATS; i++)
            {
                matrixMulNaive<<<gridDim, blockDim>>>(d_A_[gpu_id], d_B_[gpu_id], d_C_[gpu_id], N);
            }
            cudaDeviceSynchronize();

            loops += REPEATS;
        }
        roco2::metrics::utility::instance().write(loops);
    }
} // namespace kernels
} // namespace roco2
