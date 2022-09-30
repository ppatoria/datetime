#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

namespace dt {

using namespace boost::gregorian;

class date : public boost::gregorian::date {
public:
    using parent_type = boost::gregorian::date;
    using year_type = parent_type::year_type;
    using month_type = parent_type::month_type;
    using day_type = parent_type::day_type;

    date()
        : parent_type()
    {
    }

    date(const parent_type& value)
        : parent_type(value)
    {
    }

    date(const year_type& year, const month_type& month, const day_type& day)
        : parent_type(year, month, day)
    {
    }

    date(const year_type& year, int month, const day_type& day)
        : parent_type(year, boost::gregorian::date::month_type(month), day)
    {
    }

    date(size_t dayNumber)
        : parent_type(calendar_type::from_day_number(long(dayNumber)))
    {
    }

    date& operator=(const parent_type& value)
    {
        parent_type::operator=(value);
        return *this;
    }

    const parent_type& value() const { return *this; }

    parent_type& value() { return *this; }

    operator const parent_type&() const { return *this; }

    void clear() { *this = date(); }

    bool empty() const { return is_not_a_date(); }

    year_type year() const { return parent_type::year(); }

    month_type month() const { return parent_type::month(); }

    day_type day() const { return parent_type::day(); }

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

        s = to_iso_extended_string(value());
    }

    // See "date Time Input/Output" in boost documentation for a detailed
    // description of various formats
    date& fromString(const char* value, const char* format = "");

    date& fromString(const std::string& s, const char* format = "")
    {
        return fromString(s.c_str(), format);
    }

    void to_string(std::string& s, const char* format) const;

    friend std::ostream& operator<<(std::ostream& os, const date& value)
    {
        return os << value.to_string();
    }
};
} // namespace dt
