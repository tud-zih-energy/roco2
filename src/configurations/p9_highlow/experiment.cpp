#include <roco2/initialize.hpp>

#include <roco2/cpu/shell.hpp>
#include <roco2/cpu/topology.hpp>

#include <roco2/memory/numa.hpp>

#include <roco2/chrono/chrono.hpp>
#include <roco2/log.hpp>

#include <roco2/experiments/const_length.hpp>
#include <roco2/experiments/patterns/pattern_generator.hpp>

#include <roco2/kernels/high_low.hpp>

#include <roco2/task/experiment_task.hpp>
#include <roco2/task/lambda_task.hpp>
#include <roco2/task/task_plan.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <ratio>
#include <iostream>
#include <regex>
#include <stdexcept>

using namespace roco2::experiments::patterns;
using namespace std::chrono_literals;

/**
 * accepts a frequency range specification and converts it to a vector of frequencies
 * valid syntax are (ABNF, see RFC 2234)
 * FREQ_SPEC = FREQ_ITEM *("," FREQ_ITEM)
 * FREQ_ITEM = *WSP [ FREQ / FREQ_RANGE / FREQ_PLUSMINUS ] *WSP
 * FREQ = 1*DIGIT [ "." 1*DIGIT ]
 * FREQ_RANGE = 1*DIGIT "-" 1*DIGIT
 * FREQ_PLUSMINUS = 1*DIGIT "+-" 1*DIGIT
 * @param freq_spec "0.5,1991-1993,1000+-5"
 * @return [0.5, 1991, 1992, 1993, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1005]
 */
std::vector<double> get_frequencies(const std::string& freq_spec) {
    std::vector<double> freqs;

    // split on ','
    std::stringstream freq_spec_stream(freq_spec);
    for (std::string spec_item; std::getline(freq_spec_stream, spec_item, ',');) {
        // spaces are not trimmed -> include in regex
        std::regex re_empty("\\s*");
        std::regex re_only_freq("\\s*([0-9]+(?:[.][0-9]+)?)\\s*");
        std::regex re_freq_range("\\s*([0-9]+)-([0-9]+)\\s*");
        std::regex re_freq_plusminus("\\s*([0-9]+)[+]-([0-9]+)\\s*");

        std::smatch m;
        if (std::regex_match(spec_item, m, re_empty)) {
            // empty -> nop
        } else if (std::regex_match(spec_item, m, re_only_freq)) {
            freqs.push_back(std::stod(m[1]));
        } else if (std::regex_match(spec_item, m, re_freq_range)) {
            int freq_lower = stoi(m[1]);
            int freq_upper = stoi(m[2]);
            for (int freq = freq_lower; freq <= freq_upper; freq++) {
                freqs.push_back(freq);
            }
        } else if (std::regex_match(spec_item, m, re_freq_plusminus)) {
            int freq_base = stoi(m[1]);
            int variation = stoi(m[2]);
            for (int freq = freq_base - variation; freq <= (freq_base + variation); freq++) {
                freqs.push_back(freq);
            }
        } else {
            throw std::runtime_error("invalid freq spec, could not parse item: " + spec_item);
        }
    }

    return freqs;
}

void run_experiments(roco2::chrono::time_point& starting_point, bool eta_only)
{
    std::vector<roco2::kernels::high_low_bs> kernels;

    for (double freq_hz : get_frequencies(P9_HIGHLOW_FREQS)) {
        std::chrono::duration<double> period_length = std::chrono::seconds(1) / freq_hz;
        kernels.push_back(roco2::kernels::high_low_bs(period_length / 2, period_length / 2));
    }

    roco2::memory::numa_bind_local nbl; 

    // ------ EDIT GENERIC SETTINGS BELOW THIS LINE ------

    auto experiment_duration = std::chrono::seconds(20);
    auto on_list = block_pattern(176);

    // ------ EDIT GENERIC SETTINGS ABOVE THIS LINE ------

    roco2::task::task_plan plan;

#pragma omp master
    {
        roco2::log::info() << "Number of frequencies: " << kernels.size();
        roco2::log::info() << "Number of placements:  " << on_list.size() << on_list;
    }

#pragma omp barrier

    auto experiment_startpoint =
        roco2::initialize::thread(starting_point, experiment_duration, eta_only);

    roco2::experiments::const_lenght exp(experiment_startpoint, experiment_duration);

    auto experiment = [&](auto& kernel, const auto& on) {
        plan.push_back(roco2::task::experiment_task(exp, kernel, on));
    };

    // ------ EDIT TASK PLAN BELOW THIS LINE ------

    for (const auto& on : on_list) {
        // note: const kernels are not accepted by compiler
        for (auto& k : kernels) {
            experiment(k, on);
        }
    }

    // ------ EDIT TASK PLAN ABOVE THIS LINE ------

#pragma omp master
    {
        roco2::log::info() << "ETA for whole execution: "
                           << std::chrono::duration_cast<std::chrono::seconds>(plan.eta());
    }

    if (!eta_only)
    {
#pragma omp barrier

        plan.execute();
    }
}
