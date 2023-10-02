#include <roco2/initialize.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>
#include <roco2/multinode/mpi.hpp>

#include <nitro/broken_options/parser.hpp>

#include <string>
#include <vector>

#include <omp.h>

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only);

int main(int argc, char** argv)
{
    // Init MPI and first synchronize
    roco2::multinode::Mpi mpi(&argc, &argv);
    
    auto starting_point = roco2::chrono::now();

    // We synchronize the first starting point and rely on NTP time sync.
    // All subsequent synchronization points are relativ to this one.
    // That's only in the order of milliseconds, but it's fine. Right?
    starting_point = mpi.synchronize(starting_point);

    roco2::log::info() << "Starting initialize at: " << starting_point.time_since_epoch().count();

    bool eta_only;

    // first assumption:
    // GOMP_CPU_AFFINITY or respectivly KMP_AFFINITY was set to a sane value.

    nitro::broken_options::parser parser;

    parser.toggle("help").short_name("h");
    parser.toggle("debug").short_name("d");
    parser.toggle("eta_only").short_name("e");

    try
    {
        auto options = parser.parse(argc, argv);

        if (options.given("help"))
        {
            parser.usage();
            return 0;
        }

        if (options.given("debug"))
        {
            roco2::logging::filter<roco2::logging::record>::set_severity(
                nitro::log::severity_level::debug);
        }
        else
        {
            roco2::logging::filter<roco2::logging::record>::set_severity(
                nitro::log::severity_level::info);
        }

        eta_only = options.given("eta_only");
    }
    catch (nitro::broken_options::parsing_error& e)
    {
        roco2::log::warn() << e.what();
        parser.usage();
        return 1;
    }

    // TODO: What is this for?
    omp_set_dynamic(0);

    roco2::log::info() << "Starting with " << omp_get_max_threads() << " threads.";

    roco2::initialize::master();

    // Throwing exceptions out of a parellel regions doesn't seem smart.
    // Therefore we have this boolean flag here.
    bool exception_happend = false;

#pragma omp parallel default(shared) shared(starting_point, exception_happend)                     \
    firstprivate(eta_only)
    {
        try
        {
            run_experiments(starting_point, eta_only);
        }
        catch (std::exception& e)
        {
            roco2::log::fatal() << "An exception happend during execution: " << e.what();
            exception_happend = true;
        }
    }

    if (!exception_happend)
    {
        roco2::log::info() << "Successfully set your machine on fire.";

        return 0;
    }
    else
    {
        return 1;
    }
}
