#pragma once
#include <chrono>
#include <dt/TimeDuration.hpp>
#include <dt/date.hpp>
//#include "TypeTraits.h"

namespace dt {
using namespace boost::gregorian;
using namespace boost::posix_time;

class utc_date_time : public ptime {
public:
    using DataType = ptime;

    utc_date_time()
        : DataType()
    {
    }

    utc_date_time(const DataType& value)
        : DataType(value)
    {
    }

    utc_date_time(const dt::date& date, const TimeDuration& time = TimeDuration(0, 0, 0))
        : DataType(date, time)
    {
    }

    utc_date_time(const Long& value)
        : DataType(time_rep_type(value))
    {
    }

    utc_date_time(const boost::local_time::local_date_time& value)
        : DataType(value.utc_time())
    {
    }

    utc_date_time& operator=(const DataType& value)
    {
        DataType::operator=(value);
        return *this;
    }

    utc_date_time& operator=(const dt::date& value)
    {
        *this = utc_date_time(value);
        return *this;
    }

    utc_date_time& operator=(const boost::local_time::local_date_time& value)
    {
        *this = utc_date_time(value);
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
        *this = utc_date_time();
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

        return DataType::date();
    }

    TimeDuration time() const
    {
        if (empty()) {
            return TimeDuration();
        }

        return time_of_day();
    }

    static utc_date_time now()
    {
#ifdef SIMULATION

        if (_simulationTimeInitialized)
            return _simulationTime;
#endif

        return microsec_clock::universal_time();
    }

    static utc_date_time epoch()
    {
        static const DataType _epoch(boost::posix_time::from_time_t(0));
        return _epoch;
    }

    static dt::date today()
    {
        return now().date();
    }

    operator Long() const
    {
        if (empty()) {
            return Long();
        }

        // Extract internal time respresentation in date_time::base_time<ptime, posix_time_system>
        return time_.time_count();
    }

    operator std::chrono::system_clock::time_point() const
    {
        TimeDuration t = value() - epoch();

        return std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(static_cast<std::chrono::microseconds>(t)));
    }

    String toString() const
    {
        String s;
        toString(s);

        return s;
    }

    String toString(const char* format) const
    {
        String s;
        toString(s, format);

        return s;
    }

    void toString(String& s) const
    {
        if (empty()) {
            s.clear();
            return;
        }

        s = to_iso_extended_string(*this);
    }

    // See "date Time Input/Output" in boost documentation for a detailed description of various formats
    utc_date_time& fromString(const String& value, const String& format = "");
    void toString(String& s, const char* format) const;

    friend std::ostream& operator<<(std::ostream& os, const utc_date_time& value)
    {
        return os << value.toString();
    }

#ifdef SIMULATION

    static void setSimulationTime(const utc_date_time& simTime)
    {
        _simulationTime = simTime;
        _simulationTimeInitialized = true;
    }

private:
    static utc_date_time _simulationTime;
    static bool _simulationTimeInitialized;

#endif
};
#ifdef SIMULATION

utc_date_time utc_date_time::_simulationTime = utc_date_time();
bool utc_date_time::_simulationTimeInitialized = false;

#endif

static_assert(std::is_trivially_copyable<utc_date_time::DataType>::value, "Oops, why utc_date_time::DataType is not trivially copyable?!");
static_assert(std::is_trivially_copyable<utc_date_time>::value, "Oops, why utc_date_time is not trivially copyable?!");

// See "date Time Input/Output" in boost documentation for a detailed description of various formats
utc_date_time&
utc_date_time::fromString(const String& value, const String& format /* = "" */)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    using namespace boost::local_time;

    if (value.empty()) {
        clear();
        return *this;
    }

    DataType dt;
    try {
        time_input_facet facet;
        if (format.empty()) {
            facet.set_iso_extended_format();
        } else {
            facet.format(format.c_str());
        }

        std::istringstream s(value);
        s.exceptions(std::ios_base::failbit); // throw exception when parsing fails

        std::istreambuf_iterator<char> i(s), e;
        facet.get(i, e, s, dt);
    } catch (const std::out_of_range& e) {
        throw e; // TODO fix
                 // FRAMEWORK_THROW
                 // (
                 //     ConversionException,
                 //     "Failed to parse <" << value << "> as utc_date_time "
                 //     "using format <" << format << "> "
                 //     "with error <" << e.what () << ">"
                 // );
    }

    *this = dt;
    return *this;
}

void utc_date_time::toString(String& s, const char* format) const
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
        facet.put(ost, str, ' ', *this);

        s = str.str();
    } catch (const std::out_of_range& e) {
        throw e;
        // TODO fix this
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
