#include <roco2/chrono/chrono.hpp>
#include <cmath>
#include <coroutine>
#include <exception>
#include <iostream>
#include <list>
#include <vector>

#include <cblas.h>
#include <cuda.h>

struct KernelTask
{
    struct promise_type
    {
        // Called when the coroutine is created
        KernelTask get_return_object()
        {
            return KernelTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        // Called at the start of the coroutine
        std::suspend_never initial_suspend() noexcept
        {
            return {};
        }

        void return_void()
        {
        }

        // Called when the coroutine ends
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        void unhandled_exception()
        {
            std::terminate();
        }
    };

    std::coroutine_handle<promise_type> handle;

    explicit KernelTask(std::coroutine_handle<promise_type> h) : handle(h)
    {
    }

    KernelTask(const KernelTask&) = delete;
    KernelTask(KernelTask&&) = default;

    ~KernelTask()
    {
        if (handle) {
            std::cout << "destroy coro: " << handle.address() << std::endl;
            handle.destroy();
        }
    }
};

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

struct KernelRunner
{
    std::list<std::coroutine_handle<KernelTask::promise_type>> tasks_;

    bool run()
    {
        auto task = tasks_.front();
        tasks_.pop_front();

        if (!task.done())
        {
            task.resume();
        }

        return !tasks_.empty();
    }

    auto operator co_await()
    {
        struct awaiter : std::suspend_always
        {
            KernelRunner& runner_;

            explicit awaiter(KernelRunner& runner) : runner_(runner)
            {
            }

            void await_suspend(std::coroutine_handle<KernelTask::promise_type> coro) const noexcept
            {
                runner_.tasks_.push_back(coro);
            }
        };

        return awaiter{ *this };
    }
};

class CudaStream
{
    std::list<std::coroutine_handle<KernelTask::promise_type>> tasks_;
    KernelRunner& runner_;
    int gpu_;
    cudaStream_t stream_;

    static void callbackHandler(void* userData)
    {
        std::cout << "callback Handler called" << std::endl;

        CudaStream* stream = (CudaStream*)userData;

        stream->completed();
    }

public:
    CudaStream(KernelRunner& runner, int gpu) : runner_(runner), gpu_(gpu)
    {
        cudaSetDevice(gpu_);
        cudaStreamCreate(&stream_);
    }

    auto operator co_await()
    {
        struct awaiter : std::suspend_always
        {
            explicit awaiter(CudaStream& stream) : stream_(stream)
            {
            }

            CudaStream& stream_;
            void await_suspend(std::coroutine_handle<KernelTask::promise_type> coro) const noexcept
            {
                std::cout << "CudaStream::await_suspend called" << std::endl;
                cudaLaunchHostFunc(stream_.stream(), callbackHandler,
                                   const_cast<CudaStream*>(&stream_));
                CHECK_CUDA_ERROR("host launch shits")
                stream_.tasks_.push_back(coro);
            }
        };

        return awaiter{ *this };
    }

    cudaStream_t stream() const
    {
        return stream_;
    }

    KernelRunner& runner()
    {
        return runner_;
    }

private:
    void completed()
    {
        while (!tasks_.empty())
        {
            auto coro = tasks_.front();
            tasks_.pop_front();
            runner_.tasks_.push_back(coro);
        }
    }
};

KernelTask gpu_kernel(CudaStream& stream, double* A, double* B, double* C, int N, dim3 gridDim,
                      dim3 blockDim)
{
    co_await stream.runner();

    auto gpu_loops = 0ull;

    auto start = roco2::chrono::now();

    do
    {
        matrixMulNaive<<<gridDim, blockDim, 0, stream.stream()>>>(A, B, C, N);

        co_await stream;

        std::cout << " gpu loops: " << ++gpu_loops << std::endl;
    } while (roco2::chrono::now() < start + std::chrono::seconds(10));
}

KernelTask cpu_kernel(KernelRunner& runner, double* A, double* B, double* C, int Nc)
{
    co_await runner;

    auto cpu_loops = 0ull;

    auto start = roco2::chrono::now();

    do
    {

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, Nc, Nc, Nc, 1.0, A, Nc, B, Nc, 1.0,
                    C, Nc);

        //std::cout << " cpu loops: " << ++cpu_loops << std::endl;
        co_await runner;
    } while (roco2::chrono::now() < start + std::chrono::seconds(5));
}

int main()
{
    std::vector<double*> d_A_;
    std::vector<double*> d_B_;
    std::vector<double*> d_C_;

    int num_gpus;
    cudaGetDeviceCount(&num_gpus);

    int numberOfSMs;
    cudaDeviceGetAttribute(&numberOfSMs, cudaDevAttrMultiProcessorCount, 0);
    int sizeOfWarps = 32;

    std::cout << "numbeerOfSMs: " << numberOfSMs << std::endl;

    auto N = numberOfSMs * sizeOfWarps;

    std::cout << "N: " << N << std::endl;

    std::vector<double> h_A(N * N);
    std::vector<double> h_B(N * N);
    std::vector<double> h_C(N * N);

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

        cudaMemcpy(d_A_[gpu_id], h_A.data(), NUM_BYTES, cudaMemcpyHostToDevice);
        CHECK_CUDA_ERROR("cudaMemcpy")
        cudaMemcpy(d_B_[gpu_id], h_B.data(), NUM_BYTES, cudaMemcpyHostToDevice);
        CHECK_CUDA_ERROR("cudaMemcpy")
        cudaMemcpy(d_C_[gpu_id], h_C.data(), NUM_BYTES, cudaMemcpyHostToDevice);
        CHECK_CUDA_ERROR("cudaMemcpy")
    }

    auto gridDim = dim3(numberOfSMs, numberOfSMs);
    auto blockDim = dim3(sizeOfWarps / 2, sizeOfWarps / 2);

    auto gpu_id = 0;

    KernelRunner runner;

    CudaStream stream(runner, gpu_id);

    auto ck = cpu_kernel(runner, h_A.data(), h_B.data(), h_C.data(), N / 10);
    auto gk = gpu_kernel(stream, d_A_[gpu_id], d_B_[gpu_id], d_C_[gpu_id], N, gridDim, blockDim);

    while (runner.run())
    {
    }
}
