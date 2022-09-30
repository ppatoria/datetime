#pragma once

#include "utc_date_time.hpp"

namespace dt {
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

class local_date_time : public boost::local_time::local_date_time {
public:
    using DataType = boost::local_time::local_date_time;
    using time_zone_ptr = boost::local_time::time_zone_ptr;

    /// Access the local time zone
    static time_zone_ptr defaultTimeZone();

    static time_zone_ptr& timezone()
    {
        static time_zone_ptr _timeZone(defaultTimeZone());

        return _timeZone;
    }

    /// Set the local time zone
    /// It is NOT safe to call this once an application is running.  We
    /// could make it safe by protecting this object with a mutex, but it
    /// is not expected that the timezone will be changed once the
    /// application has been started.
    static void timezone(const time_zone_ptr& timeZone);

    /// Set the local time zone from a POSIX time zone specification
    static void timezone(const std::string& spec);

    local_date_time()
        : DataType(not_a_date_time)
    {
    }

    local_date_time(const DataType& value)
        : DataType(value)
    {
    }

    local_date_time(const utc_date_time& value)
        : DataType(value, timezone())
    {
    }

    local_date_time(const dt::date& date, const dt::time_duration& time = time_duration(0, 0, 0))
        : DataType(date, time, timezone(), EXCEPTION_ON_ERROR)
    {
    }
    local_date_time& operator=(const DataType& value)
    {
        DataType::operator=(value);
        return *this;
    }

    local_date_time& operator=(const dt::date& value)
    {
        *this = local_date_time(value);
        return *this;
    }

    local_date_time& operator=(const utc_date_time& value)
    {
        *this = local_date_time(value);
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

    void clear()
    {
        *this = local_date_time();
    }

    bool empty() const
    {
        return is_not_a_date_time();
    }

    dt::date date() const
    {
        if (empty()) {
            return date();
        }

        return local_time().date();
    }

    dt::time_duration time() const
    {
        if (empty()) {
            return dt::time_duration();
        }

        return local_time().time_of_day();
    }

    static local_date_time now()
    {
        return utc_date_time::now();
    }

    static dt::date today()
    {
        return now().date();
    }

    operator utc_date_time() const
    {
        return utc_date_time(*this);
    }

    std::string toString() const
    {
        std::string s;
        toString(s);

        return s;
    }

    std::string toString(const char* format) const
    {
        std::string s;
        toString(s, format);

        return s;
    }

    void toString(std::string& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }

        s = boost::posix_time::to_iso_extended_string(local_time());
    }

    // See "date Time Input/Output" in boost documentation for a detailed description of various formats
    local_date_time& fromString(const std::string& value, const std::string& format = "");
    void toString(std::string& s, const char* format) const;

    friend std::ostream& operator<<(std::ostream& os, const local_date_time& value)
    {
        std::string s;
        value.toString(s);
        return os << s;
    }
};

static_assert(!std::is_trivially_copyable<local_date_time::DataType>::value, "Oops, why local_date_time::DataType is trivially copyable?!");
static_assert(!std::is_trivially_copyable<local_date_time>::value, "Oops, why local_date_time is trivially copyable?!");
#ifndef _timezone
#define _timezone ::timezone
#endif

#ifndef _tzname
#define _tzname ::tzname
#endif

#ifndef _daylight
#define _daylight ::daylight
#endif

namespace {
    // New DST rules in effect for 2007: Second Sunday in March -> First
    // Sunday in November.  See the Energy Policy Act of 2005
    // (http://en.wikipedia.org/wiki/Energy_Policy_Act_of_2005)

    // On Windows, the 'tzname' values look like "Eastern Standard
    // Time" instead of "EST".  Take the first three upper-case
    // letters we find, or default to GMT.
    std::string
    fix_tzname(const char* tzname)
    {
        std::string fixed;

        for (; *tzname && fixed.length() < 3; ++tzname) {
            if (std::isupper(*tzname)) {
                fixed.push_back(*tzname);
            }
        }

        if (fixed.length() != 3) {
            fixed = "GMT";
        }

        return fixed;
    }

    // Trigger time zone initialization
    const local_date_time _;

} // namepsace

local_date_time::time_zone_ptr
local_date_time::defaultTimeZone()
{
    tzset();

    std::ostringstream os;

    int hours = (-1 * _timezone) / 3600;

    os << fix_tzname(_tzname[0]);

    // Ensure we have 2 digits for the hours manually inserting
    // the sign and inserting only the absolute value
    if (hours < 0) {
        os << '-';
        hours = -hours;
    } else {
        os << '+';
    }

    os << std::setw(2) << std::setfill('0') << hours;

    if (_daylight) {
        os << fix_tzname(_tzname[1]) << ",M3.2.0,M11.1.0";
    }

    return local_date_time::time_zone_ptr(new boost::local_time::posix_time_zone(os.str()));
}

/// Set the local time zone
/// It is NOT safe to call this once an application is running.  We
/// could make it safe by protecting this object with a mutex, but it
/// is not expected that the timezone will be changed once the
/// application has been started.
void local_date_time::timezone(const local_date_time::time_zone_ptr& timeZone)
{
    local_date_time::timezone() = timeZone;
}

/// Set the local time zone from a POSIX time zone specification
void local_date_time::timezone(const std::string& spec)
{
    time_zone_ptr zone(new posix_time_zone(spec));
    timezone(zone);
}

// See "date Time Input/Output" in boost documentation for a detailed description of various formats
local_date_time&
local_date_time::fromString(const std::string& value, const std::string& format /* = "" */)
{
    utc_date_time dt;
    dt.fromString(value, format);

    if (dt.empty()) {
        clear();
        return *this;
    }

    *this = local_date_time(dt.date(), dt.time());
    return *this;
}

void local_date_time::toString(std::string& s, const char* format) const
{
    using namespace boost::posix_time;

    if (NULL == format || 0 == *format) {
        return toString(s);
    }

    try {
        time_facet facet;
        facet.format(format);

        std::ostringstream str;
        str.exceptions(std::ios_base::failbit); // throw exception when parsing fails

        std::ostreambuf_iterator<char> ost(str);
        facet.put(ost, str, ' ', local_time());

        s = str.str();
    } catch (const std::out_of_range& e) {
        throw "error"; // TODO fix
                       //  FRAMEWORK_THROW
                       //  (
                       //      framework::ConversionException,
                       //      "Failed to format <" << *this << "> "
                       //      "using format <" << format << "> "
                       //      "with error <" << e.what () << ">"
                       //  );
    }
}

} // namespace dt
