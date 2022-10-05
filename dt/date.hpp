#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>

#include <stdexcept>
#include <string>

namespace dt {

using gregorian_date = boost::gregorian::date;
using gregorian_date_facet = boost::gregorian::date_facet;
using gregorian_date_input_facet = boost::gregorian::date_input_facet;

class date : public gregorian_date {
private:
    void to_string(std::string& s, const char* format) const
    {
        if (NULL == format || 0 == *format) {
            return to_string(s);
        }

        try {
            gregorian_date_facet facet;
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

    const gregorian_date& value() const { return *this; }

    gregorian_date& value() { return *this; }

public:
    using year_type = gregorian_date::year_type;
    using month_type = gregorian_date::month_type;
    using day_type = gregorian_date::day_type;

    date()
        : gregorian_date()
    {
    }

    date(const gregorian_date& value)
        : gregorian_date(value)
    {
    }

    date(const year_type& year, const month_type& month, const day_type& day)
        : gregorian_date(year, month, day)
    {
    }

    date(const year_type& year, int month, const day_type& day)
        : gregorian_date(year, gregorian_date::month_type(month), day)
    {
    }

    date(size_t dayNumber)
        : gregorian_date(calendar_type::from_day_number(long(dayNumber)))
    {
    }

    date& operator=(const gregorian_date& value)
    {
        gregorian_date::operator=(value);
        return *this;
    }

    void clear() { *this = date(); }

    bool empty() const { return is_not_a_date(); }

    year_type year() const { return gregorian_date::year(); }

    month_type month() const { return gregorian_date::month(); }

    day_type day() const { return gregorian_date::day(); }

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
        date date;
        try {
            gregorian_date_input_facet facet;
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

    date& from_string(const std::string& s, const char* format = "")
    {
        return from_string(s.c_str(), format);
    }

    friend std::ostream& operator<<(std::ostream& os, const date& value)
    {
        return os << value.to_string();
    }
};
static_assert(std::is_trivially_copyable<gregorian_date>::value, "Date and its dependencies should be trivially copyable.");
static_assert(std::is_trivially_copyable<date>::value, "Date and its dependencies should be trivially copyable.");
} // namespace dt
