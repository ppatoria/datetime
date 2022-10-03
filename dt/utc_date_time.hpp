#pragma once

#include <dt/date.hpp>
#include <dt/time_duration.hpp>
#include <dt/time_zone.hpp>
#include <stdexcept>

namespace dt {

class utc_date_time : public boost::posix_time::ptime {
private:
    const boost::posix_time::ptime& value() const { return *this; }

    boost::posix_time::ptime& value() { return *this; }

public:
    utc_date_time()
        : boost::posix_time::ptime()
    {
    }

    utc_date_time(const boost::posix_time::ptime& value)
        : boost::posix_time::ptime(value)
    {
    }

    utc_date_time(const dt::date& date,
        const dt::time_duration& time = dt::time_duration(0, 0, 0))
        : boost::posix_time::ptime(date, time)
    {
    }

    utc_date_time(const long& value)
        : boost::posix_time::ptime(time_rep_type(value))
    {
    }

    utc_date_time(const boost::local_time::local_date_time& value)
        : boost::posix_time::ptime(value.utc_time())
    {
    }

    utc_date_time& operator=(
        const boost::posix_time::ptime& value)
    {
        boost::posix_time::ptime::operator=(value);
        return *this;
    }

    utc_date_time& operator=(const dt::date& value)
    {
        *this = utc_date_time(value);
        return *this;
    }

    utc_date_time& operator=(
        const boost::local_time::local_date_time& value)
    {
        *this = utc_date_time(value);
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

        return boost::posix_time::ptime::date();
    }

    dt::time_duration time() const
    {
        if (empty()) {
            return dt::time_duration();
        }

        return time_of_day();
    }

    static utc_date_time now()
    {
#ifdef SIMULATION

        if (_simulationTimeInitialized)
            return _simulationTime;
#endif

        return boost::posix_time::microsec_clock::universal_time();
    }

    static utc_date_time epoch()
    {
        static const boost::posix_time::ptime _epoch(
            boost::posix_time::from_time_t(0));
        return _epoch;
    }

    static dt::date today()
    {
        return now().date();
    }

    operator long() const
    {
        if (empty()) {
            return long();
        }

        /** Extract internal time respresentation
         * in date_time::base_time<ptime, posix_time_system>
         */
        return time_.time_count();
    }

    operator std::chrono::system_clock::time_point() const
    {
        dt::time_duration t = value() - epoch();

        using chrono_duration = std::chrono::system_clock::time_point::duration;

        return std::chrono::system_clock::time_point(
            std::chrono::duration_cast<chrono_duration>(
                static_cast<std::chrono::microseconds>(t)));
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

        s = to_iso_extended_string(*this);
    }

    /** See "date Time Input/Output" in boost documentation
     * for a detailed description of various formats
     */
    utc_date_time& from_string(const std::string& value,
        const std::string& format = "")
    {
        using namespace boost::gregorian;
        using namespace boost::posix_time;
        using namespace boost::local_time;

        if (value.empty()) {
            clear();
            return *this;
        }

        boost::posix_time::ptime dt;
        try {
            time_input_facet facet;
            if (format.empty()) {
                facet.set_iso_extended_format();
            } else {
                facet.format(format.c_str());
            }

            std::istringstream s(value);
            /** throw exception when parsing fails */
            s.exceptions(std::ios_base::failbit);

            std::istreambuf_iterator<char> i(s), e;
            facet.get(i, e, s, dt);
        } catch (const std::out_of_range& e) {
            throw std::runtime_error(
                "Failed to parse <" + value + "> as utc_date_time "
                + "using format <" + format + "> "
                + "with error <" + e.what() + ">");
        }

        *this = dt;
        return *this;
    }

    void to_string(std::string& s, const char* format) const
    {
        using namespace boost::posix_time;

        if (NULL == format || 0 == *format) {
            return to_string(s);
        }

        try {
            time_facet facet;
            facet.format(format);

            std::ostringstream str;
            /** throw exception when parsing fails */
            str.exceptions(std::ios_base::failbit);

            std::ostreambuf_iterator<char> ost(str);
            facet.put(ost, str, ' ', *this);

            s = str.str();
        } catch (const std::out_of_range& e) {
            throw std::runtime_error(
                "Failed to format <" + to_string() + "> "
                + "using format <" + format + "> "
                + "with error <" + e.what() + ">");
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const utc_date_time& value)
    {
        return os << value.to_string();
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

static_assert(std::is_trivially_copyable<utc_date_time>::value);

} // namespace dt
