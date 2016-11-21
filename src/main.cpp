#include <roco2/initialize.hpp>

#include <roco2/log.hpp>

#include <roco2/chrono/chrono.hpp>

#include <string>

#include <omp.h>

void run_experiments(roco2::chrono::time_point starting_point, bool eta_only);

int main(int argc, char** argv)
{
    auto starting_point = roco2::chrono::now();

    roco2::log::info() << "Starting initialize at: " << starting_point.time_since_epoch().count();

    // first assumption:
    // GOMP_CPU_AFFINITY or respectivly KMP_AFFINITY was set to a sane value.
    nitro::broken_options::parser parser;

    parser.toggle("debug").short_name("d");
    parser.toggle("eta_only").short_name("e");

    auto options = parser.parse(argc, argv);

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

    // TODO: What is this for?
    omp_set_dynamic(0);

    roco2::log::info() << "Starting with " << omp_get_max_threads() << " threads.";

    roco2::initialize::master();

    // Throwing exceptions out of a parellel regions doesn't seem smart.
    // Therefore we have this boolean flag here.
    bool exception_happend = false;

#pragma omp parallel default(shared) shared(starting_point, exception_happend)
    {
        try
        {
            run_experiments(starting_point, options.given("eta_only"));
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
