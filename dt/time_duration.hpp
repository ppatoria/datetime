#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/version.hpp>

#include <chrono>

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
        return is_not_a_date_time();
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
            fractional_seconds() * 1000000 / ticks_per_second());
    }

    size_t milliseconds() const
    {
        if (empty()) {
            return static_cast<long>(long());
        }

        return static_cast<long>(
            fractional_seconds() * 1000 / ticks_per_second());
    }

    long totalMicroseconds() const
    {
        if (empty()) {
            return long();
        }

        return long(total_microseconds());
    }

    long totalMilliseconds() const
    {
        if (empty()) {
            return long();
        }

        return long(total_milliseconds());
    }

    size_t totalSeconds() const
    {
        if (empty()) {
            return static_cast<long>(long());
        }

        return total_seconds();
    }

    operator std::chrono::microseconds() const
    {
        return std::chrono::microseconds(total_microseconds());
    }

    std::string to_string() const
    {
        std::string s;
        to_string(s);

        return s;
    }

    time_duration& from_string(const std::string& value);
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

} // namespace dt
