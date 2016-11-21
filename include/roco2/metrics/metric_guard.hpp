#ifndef INCLUDE_ROCO2_METRICS_METRIC_GUARD_HPP
#define INCLUDE_ROCO2_METRICS_METRIC_GUARD_HPP

namespace roco2
{
namespace metrics
{

    template <typename Metric>
    class metric_guard
    {
    public:
        metric_guard(typename Metric::value_type value) : value(value)
        {
            Metric::instance().write(value);
        }

        ~metric_guard()
        {
            Metric::instance().write(value);
        }

        metric_guard(const metric_guard&) = delete;
        metric_guard(metric_guard&&) = delete;

        metric_guard& operator=(const metric_guard&) = delete;
        metric_guard& operator=(metric_guard&&) = delete;

    private:
        typename Metric::value_type value;
    };
}
}

#endif // INCLUDE_ROCO2_METRICS_METRIC_GUARD_HPP
