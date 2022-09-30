#pragma

#pragma once

#include <boost/date_time/local_time/local_time.hpp>
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

namespace dt {
class time_duration : public boost::posix_time::time_duration {
public:
    using DataType = boost::posix_time::time_duration;
    using HourType = DataType::hour_type;
    using MinuteType = DataType::min_type;
    using SecondType = DataType::sec_type;
    using MicroSecondType = DataType::fractional_seconds_type;

    time_duration()
        : DataType(boost::posix_time::not_a_date_time)
    {
    }

    time_duration(const DataType& value)
        : DataType(value)
    {
    }

    time_duration(HourType hour, MinuteType minute, SecondType second, MicroSecondType microsecond = 0)
        : DataType(hour, minute, second)
    {
        *this += boost::posix_time::microseconds(microsecond);
    }

    time_duration& operator=(const DataType& value)
    {
        DataType::operator=(value);
        return *this;
    }

    const DataType& value() const
    {
        return *this;
    }

    DataType& value()
    {
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

    void to_string(std::string& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }

        s = to_simple_string(*this);
    }

    std::string to_string() const
    {
        std::string s;
        to_string(s);

        return s;
    }

    time_duration& fromString(const std::string& value);
    static time_duration fromMicroseconds(MicroSecondType value)
    {
        return time_duration(0, 0, 0, value);
    }

    static time_duration fromMilliseconds(MicroSecondType value)
    {
        return fromMicroseconds(1000 * value);
    }

    static time_duration fromSeconds(SecondType value)
    {
        return time_duration(0, 0, value);
    }

    static time_duration fromMinutes(MinuteType value)
    {
        return time_duration(0, value, 0);
    }

    static time_duration fromHours(HourType value)
    {
        return time_duration(value, 0, 0);
    }

    friend std::ostream& operator<<(std::ostream& os, const time_duration& value)
    {
        return os << value.to_string();
    }
};

} // namespace dt
