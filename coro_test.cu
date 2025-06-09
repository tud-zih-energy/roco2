#include <iostream>
#include <vector>

#include <cblas.h>
#include <cuda.h>

extern __global__ void matrixMulNaive(const double* A, const double* B, double* C, int N);

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

int main()
{
    std::vector<double*> d_A_;
    std::vector<double*> d_B_;
    std::vector<double*> d_C_;

    int num_gpus;
    cudaGetDeviceCount(&num_gpus);

    int numberOfSMs;
    cuDeviceGetAttribute(&numberOfSMs, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, 0);
    int sizeOfWarps = 32;

    std::cout << "numbeerOfSMs: " << numberOfSMs << std::endl;

    auto N = numberOfSMs * sizeOfWarps;

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

    auto gridDim = dim3(numberOfSMs, numberOfSMs);
    auto blockDim = dim3(sizeOfWarps / 2, sizeOfWarps / 2);

    auto gpu_id = 0;

    cudaStream_t s;

    cudaStreamCreate(&s);
    CHECK_CUDA_ERROR("stream create")

    auto cuda_loops = 0ull;
    auto cpu_loops = 0ull;

    const auto REPEATS = 100;

    for (int i = 0; i < REPEATS; i++)
    {
        matrixMulNaive<<<gridDim, blockDim, 0, s>>>(d_A_[gpu_id], d_B_[gpu_id], d_C_[gpu_id], N);
        CHECK_CUDA_ERROR("pre-loop")
    }

    auto Nc = N / 10;

    auto i = 0;

    do
    {
        CHECK_CUDA_ERROR("loop")

        if (cudaStreamQuery(s) == cudaSuccess)
        {
            cuda_loops += REPEATS * N * N * (2 * N - 1);
            for (int i = 0; i < REPEATS; i++)
            {
                matrixMulNaive<<<gridDim, blockDim, 0, s>>>(d_A_[gpu_id], d_B_[gpu_id],
                                                            d_C_[gpu_id], N);
            }
        }
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, Nc, Nc, Nc, 1.0, h_A, Nc, h_B, Nc,
                    1.0, h_C, Nc);

        cpu_loops += Nc * Nc * (2 * Nc - 1);

        if (i++ % 100 == 0)
        {
            std::cout << "CUDA: " << cuda_loops << " cpu: " << cpu_loops << std::endl;
        }

    } while (true);
    cudaDeviceSynchronize();
}
