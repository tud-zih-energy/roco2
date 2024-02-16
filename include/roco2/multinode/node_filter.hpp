#pragma once

#include <roco2/experiments/cpu_sets/cpu_set.hpp>

#include <nitro/env/hostname.hpp>

#include <set>
#include <string>

namespace roco2
{
namespace multinode
{
    class node_filter
    {
    public:
        node_filter(const std::set<std::string>& node_list) : nodes_(node_list)
        {
        }

        friend inline experiments::cpu_sets::cpu_set
        operator|(const experiments::cpu_sets::cpu_set& set, const node_filter& filter)
        {
            auto hostname = nitro::env::hostname();

            if (filter.nodes_.count(hostname))
            {
                return set;
            }

            return {};
        }

    private:
        std::set<std::string> nodes_;
    };
} // namespace multinode
} // namespace roco2
