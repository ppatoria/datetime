#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

namespace dt {

class date : public boost::gregorian::date {
private:
    void to_string(std::string& s, const char* format) const
    {
        if (NULL == format || 0 == *format) {
            return to_string(s);
        }

        try {
            boost::gregorian::date_facet facet;
            facet.format(format);

            std::ostringstream str;
            str.exceptions(
                std::ios_base::failbit); // throw exception when parsing fails

            std::ostreambuf_iterator<char> ost(str);
            facet.put(ost, str, ' ', *this);

            s = str.str();
        } catch (const std::out_of_range& e) {
            std::runtime_error("Failed to format <" + to_string() + "> " + "using format <" + format + "> " + "with error <" + e.what() + ">");
        }
    }

    void to_string(std::string& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }

        s = to_iso_extended_string(value());
    }

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

    /**
     * See "date Time Input/Output" in boost documentation for a detailed
     * description of various formats
     **/
    date& from_string(const char* value, const char* format = "")
    {
        dt::date date;
        try {
            boost::gregorian::date_input_facet facet;
            if (NULL == format || 0 == *format) {
                facet.set_iso_extended_format();
            } else {
                facet.format(format);
            }

            std::istringstream s(value);
            s.exceptions(std::ios_base::failbit); // throw exception when parsing fails

            std::istreambuf_iterator<char> i(s), e;
            facet.get(i, e, s, date);
        } catch (const std::out_of_range& e) {
            std::runtime_error("Failed to parse <" + std::string(value) + "> as Date "
                + "using format <" + format + "> "
                + "with error <" + e.what() + ">");
        }

        *this = date;

        return *this;
    }

    dt::date& from_string(const std::string& s, const char* format = "")
    {
        return from_string(s.c_str(), format);
    }

    friend std::ostream& operator<<(std::ostream& os, const date& value)
    {
        return os << value.to_string();
    }
};
static_assert(std::is_trivially_copyable<dt::date::parent_type>::value,
    "Oops, why dt::date::parent_type is not trivially copyable?!");
static_assert(std::is_trivially_copyable<dt::date>::value,
    "Oops, why dt::date is not trivially copyable?!");
} // namespace dt
