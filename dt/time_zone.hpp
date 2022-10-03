#pragma once

#include <cctype>
#include <ctime>

#include <iomanip>

#include <boost/date_time/local_time/local_time.hpp>

namespace dt {

/**
 * https : //www.gnu.org/software/libc/manual/html_node/Time-Zone-Functions.html
 */
namespace time_zone_info {

    /**
     * This variable has a nonzero value if Daylight Saving Time rules
     * apply. A nonzero value does not neccessarily mean that Daylight
     * Saving Time is now in effect; it means only that Daylight Saving
     * Time is sometimes in effect.
     */
    inline auto is_daylight_saving() -> decltype(::daylight)
    {
#ifdef DAYLIGHT
        return DAYLIGHT;
#else
        return ::daylight;
#endif
    }

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
    struct time_zone_name {
        const char* standard_time_zone;
        const char* daylight_time_zone;
    };

    inline time_zone_name tz_name()
    {
#ifdef TZNAME
        return time_zone_name { TZNAME[0], TZNAME[1] };
#else
        return time_zone_name { ::tzname[0], ::tzname[1] };
#endif
    }

    /**
     * This contains the difference between UTC and the latest local standard
     * time, in seconds west of UTC. For example, in the U.S.Eastern time zone,
     * the value is 5*60*60.
     */
    inline auto tz_difference_from_utc() -> decltype(::timezone)
    {
#ifdef TIMEZONE
        return TIMEZONE;
#else
        return ::timezone;
#endif
    }
} // namespace tz_info
/**
 */
class time_zone {
    using time_zone_ptr = boost::local_time::time_zone_ptr;

private:
    time_zone_ptr _time_zone;

private:
    /** Access the local time zone **/
    time_zone_ptr get_default_time_zone()
    {
        using namespace boost::local_time;
        /**
         * The tzset function initializes the tzname variable from the value of the TZ
         * environment variable.
         * It is not usually necessary for your program to call this function, because
         * it is called automatically when you use the other time conversion functions
         * that depend on the time zone.
         */
        tzset();

        std::ostringstream os;

        auto tzname = time_zone_info::tz_name();

        int hours = (-1 * time_zone_info::tz_difference_from_utc()) / 3600;

        os << fix_tz_name(tzname.standard_time_zone);

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

        if (time_zone_info::is_daylight_saving()) {
            os << fix_tz_name(tzname.daylight_time_zone) << ",M3.2.0,M11.1.0";
        }

        return time_zone_ptr(new posix_time_zone(os.str()));
    }

public:
    time_zone()
        : _time_zone(get_default_time_zone())
    {
    }

    /**
     * New DST rules in effect for 2007: Second Sunday in March -> First
     * Sunday in November.  See the Energy Policy Act of 2005
     * (http://en.wikipedia.org/wiki/Energy_Policy_Act_of_2005)
     *
     * On Windows, the 'tzname' values look like "Eastern Standard
     * Time" instead of "EST".  Take the first three upper-case
     * letters we find, or default to GMT.
     **/
    std::string fix_tz_name(const char* tzname)
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

    time_zone_ptr local()
    {
        return _time_zone;
    }

    /**
     * Set the local time zone
     * It is NOT safe to call this once an application is running.  We
     * could make it safe by protecting this object with a mutex, but it
     * is not expected that the timezone will be changed once the
     * application has been started.
     **/
    void from_time_zone_ptr(const time_zone_ptr& zone)
    {
        _time_zone = zone;
    }

    /**
     * Set the local time zone from a POSIX time zone specification
     */
    void from_string(const std::string& spec)
    {
        using namespace boost::local_time;
        time_zone_ptr zone(new posix_time_zone(spec));
        _time_zone = zone;
    }
};
} // namespace dt
