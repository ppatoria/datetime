#pragma once

/* #include <String.h> */
#include <dt/TypeTraits.hpp>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/functional/hash/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>
#include <string>

namespace dt {
using String = std::string;

class Integer {
    boost::int32_t _value;

public:
    typedef boost::int32_t DataType;

    constexpr Integer()
        : _value((std::numeric_limits<DataType>::min)())
    {
    }

    Integer(const DataType& value)
        : _value(value)
    {
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
        _value = Integer();
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

    Integer& fromString(const String& s)
    {
        return fromString(s.c_str());
    }

    Integer& fromString(const char* s);

    friend std::ostream& operator<<(std::ostream& os, const Integer& value)
    {
        return os << value.toString();
    }
};

/// Special Integer class to hold enumerated values
class EnumerationValue : public Integer {
public:
    EnumerationValue()
    {
    }

    EnumerationValue(const Integer& value)
        : Integer(value)
    {
    }

    EnumerationValue& operator=(const Integer& value)
    {
        Integer::operator=(value);
        return *this;
    }

    template <typename ENUMERATION>
    operator ENUMERATION() const
    {
        ENUMERATION e;
        e = static_cast<typename ENUMERATION::DataType>(value());
        e.validate();

        return e;
    }
};

inline std::size_t hash_value(const Integer& i)
{
    return boost::hash_value(i.value());
}

} // namespace dt
