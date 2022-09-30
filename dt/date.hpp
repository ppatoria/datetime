#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

namespace dt {

using namespace boost::gregorian;

class date : public boost::gregorian::date {
public:
    using DataType = boost::gregorian::date;
    using YearType = DataType::year_type;
    using MonthType = DataType::month_type;
    using DayType = DataType::day_type;

    date()
        : DataType()
    {
    }

    date(const DataType& value)
        : DataType(value)
    {
    }

    date(const YearType& year, const MonthType& month, const DayType& day)
        : DataType(year, month, day)
    {
    }

    date(const YearType& year, int month, const DayType& day)
        : DataType(year, boost::gregorian::date::month_type(month), day)
    {
    }

    date(size_t dayNumber)
        : DataType(calendar_type::from_day_number(long(dayNumber)))
    {
    }

    date& operator=(const DataType& value)
    {
        DataType::operator=(value);
        return *this;
    }

    const DataType& value() const { return *this; }

    DataType& value() { return *this; }

    operator const DataType&() const { return *this; }

    void clear() { *this = date(); }

    bool empty() const { return is_not_a_date(); }

    YearType year() const { return DataType::year(); }

    MonthType month() const { return DataType::month(); }

    DayType day() const { return DataType::day(); }

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

        s = to_iso_extended_string(value());
    }

    // See "date Time Input/Output" in boost documentation for a detailed
    // description of various formats
    date& fromString(const char* value, const char* format = "");

    date& fromString(const std::string& s, const char* format = "")
    {
        return fromString(s.c_str(), format);
    }

    void toString(std::string& s, const char* format) const;

    friend std::ostream& operator<<(std::ostream& os, const date& value)
    {
        return os << value.toString();
    }
};
} // namespace dt
