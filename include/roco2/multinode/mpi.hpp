#pragma once

#include "roco2/chrono/util.hpp"
#include <roco2/chrono/chrono.hpp>

#define OMPI_SKIP_MPICXX 
#include <mpi.h>

namespace roco2
{

namespace multinode
{
    class Mpi
    {
    public:
        Mpi(int* argc, char*** argv)
        {
            MPI_Init(argc, argv);
            MPI_Barrier(MPI_COMM_WORLD);
        }

        roco2::chrono::time_point synchronize(roco2::chrono::time_point tp)
        {
            auto buffer = tp.time_since_epoch().count();

            MPI_Bcast(&buffer, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);

            return roco2::chrono::time_point(roco2::chrono::duration(buffer));
        }

        ~Mpi()
        {
            MPI_Finalize();
        }

    private:
    };
} // namespace multinode
} // namespace roco2
