#pragma once

#include "UtcDateTime.hpp"
#include "TypeTraits.hpp"

namespace dt{
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

class LocalDateTime : public local_date_time
{
public:

    typedef local_date_time DataType;
    typedef boost::local_time::time_zone_ptr time_zone_ptr;


    /// Access the local time zone
	static time_zone_ptr defaultTimeZone ();

    static time_zone_ptr & timezone ()
    {
		static time_zone_ptr _timeZone (defaultTimeZone ());

        return _timeZone;
    }

    /// Set the local time zone
    /// It is NOT safe to call this once an application is running.  We
    /// could make it safe by protecting this object with a mutex, but it
    /// is not expected that the timezone will be changed once the
    /// application has been started.
    static void timezone (const time_zone_ptr & timeZone);

    /// Set the local time zone from a POSIX time zone specification
    static void timezone (const std::string & spec);

    LocalDateTime ()
        : DataType (not_a_date_time)
    {
    }

    LocalDateTime (const DataType & value)
        : DataType (value)
    {
    }

    LocalDateTime (const UtcDateTime & value)
        : DataType (value, timezone ())
    {
    }

    LocalDateTime (const Date & date, const TimeDuration & time = TimeDuration (0, 0, 0))
        : DataType (date, time, timezone (), EXCEPTION_ON_ERROR)
    {
    }
    LocalDateTime & operator = (const DataType & value)
    {
        DataType::operator = (value);
        return *this;
    }

    LocalDateTime & operator = (const Date & value)
    {
        *this = LocalDateTime (value);
        return *this;
    }

    LocalDateTime & operator = (const UtcDateTime & value)
    {
        *this = LocalDateTime (value);
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

    void clear ()
    {
        *this = LocalDateTime ();
    }

    bool empty () const
    {
        return is_not_a_date_time ();
    }

    Date date () const
    {
        if (empty ())
        {
            return Date ();
        }

        return local_time ().date ();
    }

    TimeDuration time () const
    {
        if (empty ())
        {
            return TimeDuration ();
        }

        return local_time ().time_of_day ();
    }

    static
    LocalDateTime now ()
    {
        return UtcDateTime::now ();
    }

    static
    Date today ()
    {
        return now ().date ();
    }

    operator UtcDateTime () const
    {
        return UtcDateTime (*this);
    }

    String toString () const
    {
        String s;
        toString (s);
  
        return s;
    }

    String toString (const char * format) const
    {
        String s;
        toString (s, format);

        return s;
    }

    void toString (String & s) const
    {
        if (empty ())
        {
            s.clear ();
            return;
        }
        
        s = boost::posix_time::to_iso_extended_string (local_time ());
    }

    // See "Date Time Input/Output" in boost documentation for a detailed description of various formats
    LocalDateTime& fromString (const String & value, const String & format = "");
    void           toString   (String & s, const char * format) const;

    friend std::ostream& operator<< (std::ostream& os, const LocalDateTime & value)
    {
        String s;
        value.toString (s);
        return os << s;
    }
};

static_assert (! std::is_trivially_copyable<LocalDateTime::DataType>::value, "Oops, why LocalDateTime::DataType is trivially copyable?!");
static_assert (! std::is_trivially_copyable<LocalDateTime>::value, "Oops, why LocalDateTime is trivially copyable?!");
#ifndef _timezone
#define _timezone ::timezone
#endif

#ifndef _tzname
#define _tzname ::tzname
#endif

#ifndef _daylight
#define _daylight ::daylight
#endif

namespace
{
// New DST rules in effect for 2007: Second Sunday in March -> First
// Sunday in November.  See the Energy Policy Act of 2005
// (http://en.wikipedia.org/wiki/Energy_Policy_Act_of_2005)

// On Windows, the 'tzname' values look like "Eastern Standard
// Time" instead of "EST".  Take the first three upper-case
// letters we find, or default to GMT.
std::string
fix_tzname (const char * tzname)
{
    std::string fixed;

    for (; *tzname && fixed.length() < 3; ++tzname)
    {
        if (std::isupper (*tzname))
        {
            fixed.push_back (*tzname);
        }
    }

    if (fixed.length () != 3)
    {
        fixed = "GMT";
    }

    return fixed;
}

// Trigger time zone initialization
const LocalDateTime _;

} // namepsace

LocalDateTime::time_zone_ptr
LocalDateTime::defaultTimeZone ()
{
    tzset ();

    std::ostringstream os;

	int hours = (-1 * _timezone) / 3600;

    os << fix_tzname (_tzname[0]);

    // Ensure we have 2 digits for the hours manually inserting
    // the sign and inserting only the absolute value
    if (hours < 0)
    {
        os << '-';
        hours = -hours;
    }
    else
    {
        os << '+';
    }

    os << std::setw (2) << std::setfill ('0') << hours;

    if (_daylight)
    {
        os << fix_tzname (_tzname[1]) << ",M3.2.0,M11.1.0";
    }

    return LocalDateTime::time_zone_ptr (new boost::local_time::posix_time_zone (os.str ()));
}

/// Set the local time zone
/// It is NOT safe to call this once an application is running.  We
/// could make it safe by protecting this object with a mutex, but it
/// is not expected that the timezone will be changed once the
/// application has been started.
void
LocalDateTime::timezone (const LocalDateTime::time_zone_ptr & timeZone)
{
    LocalDateTime::timezone () = timeZone;
}

/// Set the local time zone from a POSIX time zone specification
void
LocalDateTime::timezone (const std::string & spec)
{
    time_zone_ptr zone (new posix_time_zone (spec));
    timezone (zone);
}

// See "Date Time Input/Output" in boost documentation for a detailed description of various formats
LocalDateTime&
LocalDateTime::fromString (const String & value, const String & format /* = "" */)
{
    UtcDateTime dt;
    dt.fromString (value, format);

    if (dt.empty ())
    {
        clear ();
        return *this;
    }

    *this = LocalDateTime (dt.date (), dt.time ());
    return *this;
}

void 
LocalDateTime::toString (String & s, const char * format) const
{
    using namespace boost::posix_time;

    if (NULL == format || 0 == *format)
    {
        return toString (s);
    }

    try
    {
        time_facet facet;
        facet.format (format);
        
        std::ostringstream str;
        str.exceptions (std::ios_base::failbit); // throw exception when parsing fails

        std::ostreambuf_iterator<char> ost(str);
        facet.put (ost, str, ' ', local_time ());

        s = str.str ();
    }
    catch (const std::out_of_range & e)
    {
      throw "error"; //TODO fix
        // FRAMEWORK_THROW
        // (
        //     framework::ConversionException,
        //     "Failed to format <" << *this << "> "
        //     "using format <" << format << "> " 
        //     "with error <" << e.what () << ">"
        // );
    }
}

} // namespace dt
