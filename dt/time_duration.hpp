#pragma once

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time/local_time/posix_time_zone.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/version.hpp>

#include <chrono>
#include <stdexcept>

#include <dt/string_util.hpp>

#if (BOOST_VERSION < 105200)
#ifndef BOOST_SUBSECOND_DURATION_OVERFLOW_BUG_FIX_APPLIED
#error Using boost (BOOST_VERSION) \
Please patch boost/date_time/time_duration.hpp as follows: \
Find the line (~270) that reads: "base_duration(0,0,0,ss*traits_type::res_adjust()/frac_of_second)" \
Insert new line: "#define BOOST_SUBSECOND_DURATION_OVERFLOW_BUG_FIX_APPLIED" \
Modify the original line (correct the multiplication order): "base_duration(0,0,0,ss*(traits_type::res_adjust()/frac_of_second))" \
For more info visit "https://svn.boost.org/trac/boost/ticket/3471"
#endif
#endif

using posix_time_duration = boost::posix_time::time_duration;
using posix_time_microseconds = boost::posix_time::microseconds;

namespace dt {
class time_duration : public posix_time_duration {
private:
    const posix_time_duration& value() const { return *this; }

    posix_time_duration& value() { return *this; }

    void to_string(std::string& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }
        s = to_simple_string(*this);
    }

public:
    using hour_type = posix_time_duration::hour_type;
    using minute_type = posix_time_duration::min_type;
    using second_type = posix_time_duration::sec_type;
    using micro_second_type = posix_time_duration::fractional_seconds_type;

    time_duration()
        : posix_time_duration(boost::posix_time::not_a_date_time)
    {
    }

    time_duration(const posix_time_duration& value)
        : posix_time_duration(value)
    {
    }

    time_duration(hour_type hour, minute_type minute, second_type second,
        micro_second_type microsecond = 0)
        : posix_time_duration(hour, minute, second)
    {
        *this += posix_time_microseconds(microsecond);
    }

    time_duration& operator=(const posix_time_duration& value)
    {
        posix_time_duration::operator=(value);
        return *this;
    }

    bool empty() const
    {
        return posix_time_duration::is_not_a_date_time();
    }

    void clear()
    {
        *this = time_duration();
    }

    size_t microseconds() const
    {
        if (empty()) {
            return static_cast<long>(long());
        }

        return static_cast<long>(
            posix_time_duration::fractional_seconds() * 1000000 / posix_time_duration::ticks_per_second());
    }

    size_t milliseconds() const
    {
        if (empty()) {
            return static_cast<long>(long());
        }

        return static_cast<long>(
            posix_time_duration::fractional_seconds() * 1000 / posix_time_duration::ticks_per_second());
    }

    long total_microseconds() const
    {
        if (empty()) {
            return long();
        }

        return long(posix_time_duration::total_microseconds());
    }

    long total_milliseconds() const
    {
        if (empty()) {
            return long();
        }

        return long(posix_time_duration::total_milliseconds());
    }

    size_t total_seconds() const
    {
        if (empty()) {
            return static_cast<long>(long());
        }

        return total_seconds();
    }

    operator std::chrono::microseconds() const
    {
        return std::chrono::microseconds(posix_time_duration::total_microseconds());
    }

    std::string to_string() const
    {
        std::string s;
        to_string(s);

        return s;
    }

    time_duration& from_string(const std::string& arg_value)
    {
        using namespace boost::algorithm;

        if (arg_value.empty() || "not-a-date-time" == arg_value) {
            clear();

            return *this;
        }

        std::string value = dt::string::to_lower_copy_of(std::cref(arg_value));

        std::string units;
        long usecFactor = 0;

        if (ends_with(value, "usec")) {
            units = "usec";
            usecFactor = 1;
        } else if (ends_with(value, "microseconds")) {
            units = "microseconds";
            usecFactor = 1;
        } else if (ends_with(value, "msec")) {
            units = "msec";
            usecFactor = 1000;
        } else if (ends_with(value, "milliseconds")) {
            units = "milliseconds";
            usecFactor = 1000;
        } else if (ends_with(value, "sec")) {
            units = "sec";
            usecFactor = 1000000LL;
        } else if (ends_with(value, "seconds")) {
            units = "seconds";
            usecFactor = 1000000LL;
        } else if (ends_with(value, "min")) {
            units = "min";
            usecFactor = 60 * 1000000LL;
        } else if (ends_with(value, "minutes")) {
            units = "minutes";
            usecFactor = 60 * 1000000LL;
        } else if (ends_with(value, "hr")) {
            units = "hr";
            usecFactor = 3600 * 1000000LL;
        } else if (ends_with(value, "hours")) {
            units = "hours";
            usecFactor = 3600 * 1000000LL;
        } else if (ends_with(value, "days")) {
            units = "days";
            usecFactor = 24 * 3600 * 1000000LL;
        }

        if (0 != usecFactor) {
            auto str_val = value.substr(0, value.size() - units.size());
            double d = dt::string::to_double(std::cref(str_val));

            *this = from_microseconds(
                static_cast<time_duration::micro_second_type>(d * usecFactor));

            return *this;
        }

        try {
            *this = boost::posix_time::duration_from_string(value);
        } catch (...) {
            throw std::runtime_error("Value <" + value + "> is not a valid time duration");
        }

        return *this;
    }

    static time_duration from_microseconds(micro_second_type value)
    {
        return time_duration(0, 0, 0, value);
    }

    static time_duration from_milliseconds(micro_second_type value)
    {
        return from_microseconds(1000 * value);
    }

    static time_duration from_seconds(second_type value)
    {
        return time_duration(0, 0, value);
    }

    static time_duration from_minutes(minute_type value)
    {
        return time_duration(0, value, 0);
    }

    static time_duration from_hours(hour_type value)
    {
        return time_duration(value, 0, 0);
    }

    friend std::ostream& operator<<(std::ostream& os, const time_duration& value)
    {
        return os << value.to_string();
    }
};
static_assert(std::is_trivially_copyable_v<posix_time_duration>);
static_assert(std::is_trivially_copyable_v<time_duration>);

} // namespace dt
