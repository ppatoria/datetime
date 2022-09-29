#pragma

#pragma once

#include <Long.hpp>
// #include <Exception.h>
#include <TypeTraits.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/version.hpp>

#include <chrono>

#if (BOOST_VERSION < 105200)
#ifndef BOOST_SUBSECOND_DURATION_OVERFLOW_BUG_FIX_APPLIED
#error \
Using boost (BOOST_VERSION) \
Please patch boost/date_time/time_duration.hpp as follows: \
Find the line (~270) that reads: "base_duration(0,0,0,ss*traits_type::res_adjust()/frac_of_second)" \
Insert new line: "#define BOOST_SUBSECOND_DURATION_OVERFLOW_BUG_FIX_APPLIED" \
Modify the original line (correct the multiplication order): "base_duration(0,0,0,ss*(traits_type::res_adjust()/frac_of_second))" \
For more info visit "https://svn.boost.org/trac/boost/ticket/3471"
#endif
#endif

using namespace boost::posix_time;

class TimeDuration : public time_duration
{
public:

    typedef time_duration DataType;

    typedef DataType::hour_type                 HourType;
    typedef DataType::min_type                  MinuteType; 
    typedef DataType::sec_type                  SecondType;
    typedef DataType::fractional_seconds_type   MicroSecondType;


    TimeDuration ()
        : DataType (not_a_date_time)
    {
    }

    TimeDuration (const DataType & value)
        : DataType (value)
    {
    }

    TimeDuration (HourType hour, MinuteType minute, SecondType second, MicroSecondType microsecond = 0)
        : DataType (hour, minute, second)
    {
        *this += boost::posix_time::microseconds (microsecond);
    }

    TimeDuration & operator = (const DataType & value)
    {
        DataType::operator = (value);
        return *this;
    }

    const DataType & value () const
    {
        return *this;
    }

    DataType & value ()
    {
        return *this;
    }

    bool empty () const
    {
        return is_not_a_date_time ();
    }

    void clear ()
    {
        *this = TimeDuration ();
    }

    size_t microseconds () const
    {
        if (empty ())
        {
            return static_cast<long> (Long ());
        }

        return static_cast<long>
        (
            fractional_seconds () * 1000000 /
            ticks_per_second ()
        );
    }

    size_t milliseconds () const
    {
        if (empty ())
        {
            return static_cast<long> (Long ());
        }

        return static_cast<long>
        (
            fractional_seconds () * 1000 /
            ticks_per_second ()
        );
    }

    Long totalMicroseconds () const
    {
        if (empty ())
        {
			return Long ();
		}
		
        return Long (total_microseconds ());
    }

    Long totalMilliseconds () const
    {
        if (empty ())
        {
			return Long ();
		}

        return Long (total_milliseconds ());
    }

    size_t totalSeconds () const
    {
        if (empty ())
        {
            return static_cast<long> (Long ());
		}

        return total_seconds ();
    }

    operator std::chrono::microseconds () const
    {
        return std::chrono::microseconds (total_microseconds());
    }

    void toString (String & s) const
    {
        if (empty ())
        {
            s.clear ();
            return;
        }

        s = to_simple_string (*this);
    }

    String toString () const
    {
        String s;
        toString (s);

        return s;
    }

    TimeDuration & fromString (const String & value);

    static TimeDuration fromMicroseconds (MicroSecondType value)
    {
        return TimeDuration (0, 0, 0, value);
    }

    static TimeDuration fromMilliseconds (MicroSecondType value)
    {
        return fromMicroseconds (1000 * value);
    }

    static TimeDuration fromSeconds (SecondType value)
    {
        return TimeDuration (0, 0, value);
    }

    static TimeDuration fromMinutes (MinuteType value)
    {
        return TimeDuration (0, value, 0);
    }

    static TimeDuration fromHours (HourType value)
    {
        return TimeDuration (value, 0, 0);
    }

    friend std::ostream & operator<< (std::ostream & os, const TimeDuration & value)
    {
        return os << value.toString ();
    }
};

