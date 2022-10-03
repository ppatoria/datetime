#pragma once
#include <dt/time_zone.hpp>
/**
https://www.gnu.org/software/libc/manual/html_node/Time-Zone-Functions.html
**/

/**
 * This contains the difference between UTC and the latest local standard time,
 * in seconds west of UTC.
 * For example, in the U.S.Eastern time zone, the value is 5*60*60.
 */
#include <ctime>

#ifndef _timezone
#define _timezone ::timezone
#endif

/**
 * The array tzname contains two strings, which are the standard names of the pair
 * of time zones (standard and Daylight Saving) that the user has selected.
 * tzname[0] is the name of the standard time zone (for example, "EST"), and
 * tzname[1] is the name for the time zone when Daylight Saving Time is in use (for
 * example, "EDT"). These correspond to the std and dst strings (respectively) from
 * the TZ environment variable. If Daylight Saving Time is never used, tzname[1] is
 * the empty string.

 * The tzname array is initialized from the TZ environment variable whenever tzset,
 * ctime, strftime, mktime, or localtime is called. If multiple abbreviations have
 * been used (e.g. "EWT" and "EDT" for U.S. Eastern War Time and Eastern Daylight
 * Time), the array contains the most recent abbreviation.
*/
#ifndef _tzname
#define _tzname ::tzname
#endif

/**
 * This variable has a nonzero value if Daylight Saving Time rules apply.
 * A nonzero value does not neccessarily mean that Daylight Saving Time is now in effect;
 * it means only that Daylight Saving Time is sometimes in effect.
 */
#ifndef _daylight
#define _daylight ::daylight
#endif

struct time_zone_info {

    auto daylight() const -> decltype(::daylight)
    {
#ifdef _daylight
        return _daylight;
#else
        return ::daylight;
#endif
    }
    //   auto tzname() const -> decltype(::tzname)
    //     {
    // #ifdef _tzname
    //         return _tzname;
    // #else
    //         return ::tzname;
    // #endif
    //     }

    auto timezone() const -> decltype(::timezone)
    {
#ifdef _timezone
        return _timezone;
#else
        return ::timezone;
#endif
    }
};

#include "utc_date_time.hpp"
namespace dt {

/**
 * New DST rules in effect for 2007: Second Sunday in March -> First
 * Sunday in November.  See the Energy Policy Act of 2005
 * (http://en.wikipedia.org/wiki/Energy_Policy_Act_of_2005)
 *
 * On Windows, the 'tzname' values look like "Eastern Standard
 * Time" instead of "EST".  Take the first three upper-case
 * letters we find, or default to GMT.
 **/
std::string fix_tzname(const char* tzname)
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

class local_date_time : public boost::local_time::local_date_time {
public:
    using boost_local_date_time = boost::local_time::local_date_time;
    using time_zone_ptr = boost::local_time::time_zone_ptr;

    /** Access the local time zone **/
    static time_zone_ptr default_time_zone()
    {
        /**
         * The tzset function initializes the tzname variable from the value of the TZ
         * environment variable.
         * It is not usually necessary for your program to call this function, because it
         * is called automatically when you use the other time conversion functions that
         * depend on the time zone.
         */
        tzset();

        std::ostringstream os;

        int hours = (-1 * _timezone) / 3600;

        os << fix_tzname(_tzname[0]);

        /**
         * Ensure we have 2 digits for the hours manually inserting
         * the sign and inserting only the absolute value
         **/
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

        return local_date_time::time_zone_ptr(
            new boost::local_time::posix_time_zone(os.str()));
    }

    static time_zone_ptr& timezone()
    {
        static time_zone_ptr _timeZone(default_time_zone());

        return _timeZone;
    }

    /**
     * Set the local time zone
     * It is NOT safe to call this once an application is running.  We
     * could make it safe by protecting this object with a mutex, but it
     * is not expected that the timezone will be changed once the
     * application has been started.
     **/
    static void timezone(const time_zone_ptr& timeZone);

    /** Set the local time zone from a POSIX time zone specification **/
    static void timezone(const std::string& spec);

    local_date_time()
        : boost_local_date_time(not_a_date_time)
    {
    }

    local_date_time(const boost_local_date_time& value)
        : boost_local_date_time(value)
    {
    }

    local_date_time(const utc_date_time& value)
        : boost_local_date_time(value, timezone())
    {
    }

    local_date_time(const dt::date& date, const dt::time_duration& time = time_duration(0, 0, 0))
        : boost_local_date_time(date, time, timezone(), EXCEPTION_ON_ERROR)
    {
    }
    local_date_time& operator=(const boost_local_date_time& value)
    {
        boost_local_date_time::operator=(value);
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

    const boost_local_date_time& value() const
    {
        return *this;
    }

    boost_local_date_time& value()
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

    std::string to_string() const
    {
        std::string s;
        to_string(s);

        return s;
    }

    std::string to_string(const char* format) const
    {
        std::string s;
        to_string(s, format);

        return s;
    }

    void to_string(std::string& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }

        s = boost::posix_time::to_iso_extended_string(local_time());
    }

    // See "date Time Input/Output" in boost documentation for a detailed description of various formats
    local_date_time& fromString(const std::string& value, const std::string& format = "");
    void to_string(std::string& s, const char* format) const;

    friend std::ostream& operator<<(std::ostream& os, const local_date_time& value)
    {
        std::string s;
        value.to_string(s);
        return os << s;
    }
};

static_assert(!std::is_trivially_copyable<local_date_time::boost_local_date_time>::value, "Oops, why local_date_time::DataType is trivially copyable?!");
static_assert(!std::is_trivially_copyable<local_date_time>::value, "Oops, why local_date_time is trivially copyable?!");

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
    time_zone_ptr zone(new boost::local_time::posix_time_zone(spec));
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

void local_date_time::to_string(std::string& s, const char* format) const
{
    using namespace boost::posix_time;

    if (NULL == format || 0 == *format) {
        return to_string(s);
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
/** Trigger time zone initialization **/
const local_date_time _;

} // namespace dt
