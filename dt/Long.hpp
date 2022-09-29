#pragma once

#include <limits>

#pragma once

#include <dt/Integer.hpp>
#include <dt/TypeTraits.hpp>

namespace dt {
using boost::int64_t;
using String = std::string;
class Long {
    int64_t _value;

public:
    typedef int64_t DataType;

    constexpr Long()
        : _value((std::numeric_limits<DataType>::min)())
    {
    }

    Long(const DataType& value)
        : _value(value)
    {
    }

    // Need this overload to verify emptiness
    Long(const Integer& value)
    {
        *this = value;
    }

    Long& operator=(const DataType& value)
    {
        _value = value;
        return *this;
    }

    // Need this overload to verify emptiness
    Long& operator=(const Integer& value)
    {
        if (value.empty()) {
            clear();
        } else {
            _value = value;
        }

        return *this;
    }

    operator const DataType&() const
    {
        return _value;
    }

    operator DataType&()
    {
        return _value;
    }

    const DataType& value() const
    {
        return _value;
    }

    DataType& value()
    {
        return _value;
    }

    void clear()
    {
        _value = Long();
    }

    bool isZero() const
    {
        return 0 == _value;
    }

    static bool empty(const DataType& value)
    {
        return (std::numeric_limits<DataType>::min)() == value;
    }

    bool empty() const
    {
        return empty(_value);
    }

    String toString() const
    {
        String s;
        toString(s);
        return s;
    }

    void toString(String& s) const;

    Long& fromString(const String& s)
    {
        return fromString(s.c_str());
    }

    Long& fromString(const char* s);

    friend std::ostream& operator<<(std::ostream& os, const Long& value)
    {
        return os << value.toString();
    }
};
} // namespace dt
