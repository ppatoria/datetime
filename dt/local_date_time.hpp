#pragma once
#include "time_zone.hpp"
#include "utc_date_time.hpp"
#include <dt/time_zone.hpp>
#include <stdexcept>
namespace dt {

class local_date_time : public boost::local_time::local_date_time {

public:
    using boost_local_date_time = boost::local_time::local_date_time;

    local_date_time()
        : boost_local_date_time(boost::date_time::not_a_date_time)
    {
    }

    local_date_time(const boost_local_date_time& value)
        : boost_local_date_time(value)
    {
    }

    local_date_time(const utc_date_time& value)
        : boost_local_date_time(
            value,
            time_zone().local())
    {
    }

    local_date_time(const dt::date& date,
        const dt::time_duration& time = time_duration(0, 0, 0))
        : boost_local_date_time(
            date,
            time,
            time_zone().local(),
            EXCEPTION_ON_ERROR)
    {
    }

    bool is_daylight_saving()
    {
        return dt::time_zone_info::is_daylight_saving();
    }

    dt::time_zone_info::time_zone_name tz_name()
    {
        return dt::time_zone_info::tz_name();
    }

    auto tz_difference_from_utc()
    {
        return dt::time_zone_info::tz_difference_from_utc();
    }

    local_date_time&
    operator=(const boost_local_date_time& value)
    {
        boost_local_date_time::operator=(value);
        return *this;
    }

    local_date_time&
    operator=(const dt::date& value)
    {
        *this = local_date_time(value);
        return *this;
    }

    local_date_time&
    operator=(const utc_date_time& value)
    {
        *this = local_date_time(value);
        return *this;
    }

    const boost_local_date_time&
    value() const
    {
        return *this;
    }

    boost_local_date_time&
    value()
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

    dt::time_duration
    time() const
    {
        if (empty()) {
            return dt::time_duration();
        }

        return local_time().time_of_day();
    }

    static local_date_time
    now()
    {
        return utc_date_time::now();
    }

    static dt::date
    today()
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

    /**
       See "date Time Input/Output" in boost documentation for a detailed description of various formats
    */
    local_date_time& from_string(
        const std::string& value,
        const std::string& format = "")
    {
        utc_date_time dt;
        dt.from_string(value, format);

        if (dt.empty()) {
            clear();
            return *this;
        }

        *this = local_date_time(dt.date(), dt.time());
        return *this;
    }

    void to_string(
        std::string& s,
        const char* format) const
    {
        using namespace boost::posix_time;

        if (NULL == format || 0 == *format) {
            return to_string(s);
        }

        try {
            time_facet facet;
            facet.format(format);

            std::ostringstream str;
            str.exceptions(
                std::ios_base::failbit); // throw exception when parsing fails

            std::ostreambuf_iterator<char> ost(str);
            facet.put(ost, str, ' ', local_time());

            s = str.str();
        } catch (const std::out_of_range& e) {
            throw std::runtime_error("Failed to format <" + to_string() + "> "
                + "using format <" + format + "> "
                + "with error <" + e.what() + ">");
        }
    }

    friend std::ostream&
    operator<<(std::ostream& os,
        const local_date_time& value)
    {
        std::string s;
        value.to_string(s);
        return os << s;
    }
};

static_assert(!std::is_trivially_copyable<local_date_time::boost_local_date_time>::value);
static_assert(!std::is_trivially_copyable<local_date_time>::value);

/** Trigger time zone initialization **/
const local_date_time _;

} // namespace dt
