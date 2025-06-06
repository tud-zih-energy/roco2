#include <roco2/kernels/naive_mm_gpu_kernel.hpp>
#include <roco2/log.hpp>
#include <roco2/metrics/utility.hpp>

#include <ostream>

#include <omp.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

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

#define CHECK_CUDA_ERROR(msg)                                                                      \
    {                                                                                              \
        cudaError_t err = cudaGetLastError();                                                      \
        if (err != cudaSuccess)                                                                    \
        {                                                                                          \
            FILE* log_file = fopen("crash_log.txt", "a");                                          \
            if (log_file)                                                                          \
            {                                                                                      \
                fprintf(log_file, "CUDA Error: %s\n", msg);                                        \
                fprintf(log_file, "File: %s\n", __FILE__);                                         \
                fprintf(log_file, "Line: %d\n", __LINE__);                                         \
                fprintf(log_file, "Error: %s\n", cudaGetErrorString(err));                         \
                fclose(log_file);                                                                  \
            }                                                                                      \
            printf("CUDA Error: %s\nFile: %s\nLine: %d\nError: %s\n", msg, __FILE__, __LINE__,     \
                   cudaGetErrorString(err));                                                       \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    }

namespace roco2
{
namespace kernels
{
    int N;
    dim3 gridDim;
    dim3 blockDim;

    naive_mm_gpu_kernel::naive_mm_gpu_kernel()
    {
        if (omp_get_thread_num() == 0)
        {
            int num_gpus;
            cudaGetDeviceCount(&num_gpus);

            int numberOfSMs;
            cuDeviceGetAttribute(&numberOfSMs, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, 0);
            int sizeOfWarps = 32;

            std::cout << "numbeerOfSMs: " << numberOfSMs << std::endl;

            N = numberOfSMs * sizeOfWarps;

            std::cout << "N: " << N << std::endl;

 
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
                CHECK_CUDA_ERROR("cudaSetDevice")

                d_A_.push_back(nullptr);
                d_B_.push_back(nullptr);
                d_C_.push_back(nullptr);

                cudaMalloc(&(d_A_[gpu_id]), NUM_BYTES);
                CHECK_CUDA_ERROR("cudaMalloc")
                cudaMalloc(&(d_B_[gpu_id]), NUM_BYTES);
                CHECK_CUDA_ERROR("cudaMalloc")
                cudaMalloc(&(d_C_[gpu_id]), NUM_BYTES);
                CHECK_CUDA_ERROR("cudaMalloc")

                cudaMemcpy(d_A_[gpu_id], h_A, NUM_BYTES, cudaMemcpyHostToDevice);
                CHECK_CUDA_ERROR("cudaMemcpy")
                cudaMemcpy(d_B_[gpu_id], h_B, NUM_BYTES, cudaMemcpyHostToDevice);
                CHECK_CUDA_ERROR("cudaMemcpy")
                cudaMemcpy(d_C_[gpu_id], h_C, NUM_BYTES, cudaMemcpyHostToDevice);
                CHECK_CUDA_ERROR("cudaMemcpy")
            }

            delete[] h_A;
            delete[] h_B;
            delete[] h_C;

           gridDim = dim3(numberOfSMs, numberOfSMs);
            blockDim = dim3(sizeOfWarps / 2, sizeOfWarps / 2);
        }
    }

    naive_mm_gpu_kernel::~naive_mm_gpu_kernel()
    {
        if (omp_get_thread_num() == 0)
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
    }

    void naive_mm_gpu_kernel::run_kernel(int gpu_id)
    {
        std::cout << "cuda thread started for device: " << gpu_id << std::endl;

        cudaSetDevice(gpu_id);

        std::size_t loops = 0;

        const auto REPEATS = 10;

        while (running_)
        {
            for (int i = 0; i < REPEATS; i++)
            {
                matrixMulNaive<<<gridDim, blockDim>>>(d_A_[gpu_id], d_B_[gpu_id], d_C_[gpu_id], N);
            }
            cudaDeviceSynchronize();
            CHECK_CUDA_ERROR("cudaDeviceSync")

            loops += REPEATS;
            log::info() << loops << " loops done";
        }
        roco2::metrics::utility::instance().write(loops);
    }
} // namespace kernels
} // namespace roco2
