#pragma once

#include <string>
#include <assert.h>
#include <functional>
#include <algorithm>
#include <cstring>
#include <cstdarg>

#include <boost/algorithm/string.hpp>

#include "Types.h"

#ifdef _WIN32

#undef  max
#undef  min

#define strcasecmp  _stricmp

#endif // #ifdef _WIN32

//Test for Magic
//#define CADRE_STRING_MIN_BUILTIN_CAPACITY 16

#ifndef CADRE_STRING_MIN_BUILTIN_CAPACITY

#define CADRE_STRING_MIN_BUILTIN_CAPACITY 32

#endif // #ifndef CADRE_STRING_MIN_BUILTIN_CAPACITY

#ifndef CADRE_STRING_MAX_BUILTIN_CAPACITY

#define CADRE_STRING_MAX_BUILTIN_CAPACITY 4096

#endif // #ifndef CADRE_STRING_MAX_BUILTIN_CAPACITY

namespace Cadre
{
namespace Data
{

class MediumString;

class String
{
public:

    typedef std::string::size_type size_type;

    static constexpr size_type EMPTY_POSITION = std::string::npos; //-1;
    static constexpr size_type npos = EMPTY_POSITION;

    enum
    {
        BUFFER_SIZE        = CADRE_STRING_MIN_BUILTIN_CAPACITY,
        MAX_BUILTIN_LENGTH = 8192 - 2, /* 13 bits, 1 1111 1111 1110 */
        HEAP_STORAGE_MASK  = ((MAX_BUILTIN_LENGTH + 1) << 3), /* 1111 1111 1111 1000 */
        CAPACITY_MASK      = 3, // 0011
        LOCKED_MASK        = 4  // 0100
    };

    using TypeDescriptor = Cadre::Data::TypeDescriptor<String, void, ElementaryType, false, true>;

protected:

    template<std::uint16_t CAPACITY>
    struct BuiltinCapacity
    {
    };

private:

#pragma pack (push)
#pragma pack (1)

    // Left 13 bits are reserved for storing string length of up to MAX_BUILTIN_LENGTH 
    // If string is too long to fit in built-in buffer, left 13 bits are all 1, i.e. length = MAX_BUILTIN_LENGTH + 1
    // 2 right bits are reserved for storing encoded built-in capacity
    // 3rd from right bit is reserved for storing <locked> status
    std::uint16_t _flags;

    mutable union
    {
        // Used when string fits inside
        char builtin [BUFFER_SIZE + 1];

        // Used when string is too long to fit inside built-in buffer
        struct
        {
            std::uint32_t _capacity;
            std::uint32_t _length;
            char *          _value;
        } heap;
    } _storage;

#pragma pack (pop)

    class va_guard
    {
        va_list & _args;

    public:

        va_guard (va_list & args)
            : _args (args)
        {
        }

       ~va_guard ()
        {
            va_end (_args);
        }
    };

    std::uint16_t builtinCapacity_ () const
    {
        const std::uint16_t capacityFlags (_flags & CAPACITY_MASK);
        
        const std::uint16_t capacity
        (
            capacityFlags == 0 /* 0000 */ ?     CADRE_STRING_MIN_BUILTIN_CAPACITY :
           (capacityFlags == 1 /* 0001 */ ? 2 * CADRE_STRING_MIN_BUILTIN_CAPACITY :
           (capacityFlags == 2 /* 0010 */ ? 256                                   :
           (capacityFlags == 3 /* 0011 */ ?     CADRE_STRING_MAX_BUILTIN_CAPACITY : 0)))
        );

        assert (0 != capacity);

        return capacity;
    }

    void length_ (size_t length)
    {
        assert (length <= capacity ());

        if (isUsingBuiltinStorage ())
        {
            assert (length <= MAX_BUILTIN_LENGTH);

            _flags = static_cast<std::uint16_t>(length << 3) | (_flags & (LOCKED_MASK | CAPACITY_MASK));

            _storage.builtin [length] = 0;
        }
        else
        {
            _storage.heap._length         = static_cast<std::uint32_t>(length);
            _storage.heap._value [length] = 0;
        }

        assert (length == size ());
    }

    template<std::uint16_t CAPACITY>
    constexpr void construct_ (const BuiltinCapacity<CAPACITY> &)
    {
        static_assert
        (
            CADRE_STRING_MAX_BUILTIN_CAPACITY <= MAX_BUILTIN_LENGTH,
            "Only 13 bits are available to store length in _flags"
        );

        static_assert
        (
            CAPACITY == CADRE_STRING_MIN_BUILTIN_CAPACITY ||
            CAPACITY == 2 * CADRE_STRING_MIN_BUILTIN_CAPACITY ||
            CAPACITY == 256 ||
            CAPACITY == CADRE_STRING_MAX_BUILTIN_CAPACITY,
            "Invalid CAPACITY"
        );

        _flags = (CAPACITY ==     CADRE_STRING_MIN_BUILTIN_CAPACITY ? 0 /* 0000 */ :
                 (CAPACITY == 2 * CADRE_STRING_MIN_BUILTIN_CAPACITY ? 1 /* 0001 */ :
                 (CAPACITY == 256                                   ? 2 /* 0010 */ :
                 (CAPACITY ==     CADRE_STRING_MAX_BUILTIN_CAPACITY ? 3 /* 0011 */ : 0 ))));

        _storage.builtin [0] = 0;

        assert (isUsingBuiltinStorage ());
        assert (CAPACITY == builtinCapacity_ ());
    }

    void lock_ (const char * id);

protected:

    template<std::uint16_t CAPACITY>
    String (const BuiltinCapacity<CAPACITY> & capacity)
    {
        construct_ (capacity);
    }

public:

   ~String ()
    {
        if (! isUsingBuiltinStorage ())
        {
            assert (NULL != _storage.heap._value);

            free (_storage.heap._value);

            _storage.heap._value = NULL;
        }
    }

    String ()
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());
    }

    String (const String & from)
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());

        assign (from);
    }

    String (const char * from, size_t length)
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());

        assign (from, length);
    }

    String (const char * from)
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());

        if (NULL != from)
        {
            assign (from);
        }
    }

    explicit
    String (char from)
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());

        assign (&from, 1);
    }

    String (const std::string & from)
    {
        construct_ (BuiltinCapacity<BUFFER_SIZE> ());

        assign (from);
    }
    String(const std::string_view & from)
    {
        construct_(BuiltinCapacity<BUFFER_SIZE>());

        assign(from);
    }

    bool isUsingBuiltinStorage () const
    {
        return HEAP_STORAGE_MASK != (_flags & HEAP_STORAGE_MASK);
    }

    size_t capacity () const
    {
        return isUsingBuiltinStorage () ? builtinCapacity_ () : _storage.heap._capacity;
    }

    bool isLocked () const
    {
        return 0 != (_flags & LOCKED_MASK);
    }

    bool empty () const
    {
        return 0 == length ();
    }

    size_t length () const
    {
        return isUsingBuiltinStorage () ? size_t (_flags >> 3) : _storage.heap._length;
    }

    size_t size () const
    {
        return length ();
    }

    const char * c_str () const
    {
        return begin ();
    }

    const char * data () const
    {
        return begin ();
    }

    const char * begin () const
    {
        return isUsingBuiltinStorage () ? _storage.builtin : _storage.heap._value;
    }

    char * begin ()
    {
        return isUsingBuiltinStorage () ? _storage.builtin : _storage.heap._value;
    }

    const char * rbegin () const
    {
        return begin () + length () - 1;
    }

    char * rbegin ()
    {
        return begin () + length () - 1;
    }

    const char * end () const
    {
        return begin () + length ();
    }

    const char * rend () const
    {
        return begin () - 1;
    }

    char * end ()
    {
        return begin () + length ();
    }

    char * rend ()
    {
        return begin () - 1;
    }

    void clear ()
    {
        length_ (0);
    }

    void lock (const char * id)
    {
        if (! isLocked ())
        {
            lock_ (id);
        }

        assert (isUsingBuiltinStorage ());
    }
    
    void lock ()
    {
        return lock (NULL);
    }

    void reserve (size_t capacity, bool preserve = true);

    String & operator = (const String & from)
    {
        return assign (from);
    }

    String & operator = (const char * from)
    {
        return assign (from);
    }

    String & operator = (const std::string & from)
    {
        return assign (from);
    }
    String & operator = (const std::string_view & from)
    {
        return assign(from);
    }

    String & assign (const String & from)
    {
        return copy (from.c_str (), from.size ());
    }

    String & assign (const char * from)
    {
        if (NULL == from)
        {
            clear ();

            return *this;
        }

        return copy (from, std::strlen (from));
    }

    String & assign (const char * sz, size_t length)
    {
        return copy (sz, length);
    }

    String & assign (const std::string & from)
    {
        return copy (from.c_str (), from.size ());
    }

    String & assign(const std::string_view & from)
    {
        return copy(from.data(), from.size());
    }

    String & copy   (const char * sz, size_t length);
    String & append (const char * sz, size_t length);

    String & append (const String & s)
    {
        return append (s.c_str (), s.size ());
    }

    String & append (const std::string & s)
    {
        return append (s.c_str (), s.size ());
    }

    String & append(const std::string_view & s)
    {
        return append(s.data(), s.size());
    }

    String & append (const char * s)
    {
        return append (s, std::strlen (s));
    }

    String & append(char c)
    {
        assert(size() <= capacity());
        assert(0 == *end());

        std::size_t currentLength(length());
        if (BOOST_UNLIKELY(currentLength + 1 > capacity()))
        {
            reserve(currentLength + 1);
        }
        *(begin() + currentLength) = c;
        length_(currentLength + 1);

        assert(size() <= capacity());
        assert(0 == *end());

        return *this;
    }
    String & format (const char * format, ...)
    {
        va_list args;
        va_start (args, format);

        va_guard guard (args);

        vformat (format, args);

        return *this;
    }

    void vformat (const char * format, va_list & args);

    MediumString substr (size_t pos, size_t count = EMPTY_POSITION) const;

    size_type find       (const String & pattern, const size_t start = 0) const;
    size_type find_first (const String & pattern) const;
    size_type find_last  (const String & pattern) const;

    size_type find_replace (const String & pattern, const String & replacement, const size_t start);

    String & replace (size_t pos, size_t count, const String & replacement);

    bool find_replace       (const String & pattern, const String & replacement);
    bool find_replace_first (const String & pattern, const String & replacement);
    bool find_replace_last  (const String & pattern, const String & replacement);
    bool find_replace_all   (const String & pattern, const String & replacement);

    bool replace_all (char from, char to);

    bool to_lower ();
    bool to_upper ();
    MediumString to_lower_copy () const;
    MediumString to_upper_copy () const;

    String & trim_front ();
    String & trim_front (char c);
    String & trim_back  ();
    String & trim_back  (char c);

    String & trim ()
    {
        return trim_front ().trim_back ();
    }
    
    String & sprintf (const char * format, ...)
    {
        va_list args;
        va_start (args, format);
        va_guard guard (args);
        
        vsprintf (format, args);
        
        return *this;
    }

    void vsprintf (const char * format, va_list & args)
    {
        clear ();

        return vformat (format, args);
    }

    String & erase (size_t pos, size_t count);

    const char & at (size_t pos) const;

    char & at (size_t pos)
    {
        const String & s (*this);
        
        return const_cast<char &> (s.at (pos));
    }

    char & operator [] (size_t pos)
    {
        return at (pos);
    }

    const char & operator [] (size_t pos) const
    {
        return at (pos);
    }

    String & truncate (size_t length)
    {
        if (length < size ())
        {
            length_ (length);
        }
        
        return *this;
    }

    bool equal_to (const char * sz, size_t length) const
    {
        if (size   () != length ||
            *c_str () != *sz    ||
            0 != std::strncmp (c_str (), sz, length))
        {
            return false;
        }

        return true;
    }

    int compare (const char * sz, size_t length) const
    {
        return std::strncmp (c_str (), sz, std::max (length, size ()));
    }

    int compare (const String & pattern) const
    {
        return compare (pattern.c_str (), pattern.size ());
    }

    int compare_nocase (const String & pattern) const;

    int compare_nocase (const char * sz, size_t length) const
    {
        // Assume speed is not an issue
        return compare_nocase (String (sz, length));
    }

    bool starts_with (const String & pattern) const
    {
        return pattern.size () <= size () && 
               0 == std::strncmp (c_str (), pattern.c_str (), pattern.size ());
    }

    bool ends_with (const String & pattern) const
    {
        return pattern.size () <= size () &&
               0 == std::strncmp (rbegin () - pattern.size () + 1, pattern.c_str (), pattern.size ());
    }

    const String & value () const
    {
        return *this;
    }

    String & value ()
    {
        return *this;
    }

    String & fromString (const String & value)
    {
        return assign (value);
    }

    String & fromString (const char * value)
    {
        return assign (value);
    }

    String & fromString (const std::string & value)
    {
        return assign (value);
    }

    String & fromString(const std::string_view & value)
    {
        return assign(value);
    }

    const String & toString () const
    {
        return *this;
    }

    void toString (String & value) const
    {
        value.assign (*this);
    }

    operator std::string () const
    {
        return std::string (c_str (), size ());
    }

    operator std::string_view() const
    {
        return std::string_view(c_str(), size());
    }

    friend inline std::ostream &
    operator << (std::ostream & o, const String & s)
    {
        o.write (s.c_str (), s.size ());

        return o;
    }
};

typedef String ShortString;

class RegularString : public String
{
protected:

    template<std::uint16_t CAPACITY>
    RegularString (const BuiltinCapacity<CAPACITY> & capacity)
        : String (capacity)
    {
    }

public:

    enum { BUFFER_SIZE = 2 * CADRE_STRING_MIN_BUILTIN_CAPACITY };

    using TypeDescriptor = Cadre::Data::TypeDescriptor<RegularString, String, ElementaryType, false, true>;

    RegularString ()
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
    }

    RegularString (const RegularString & value)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (value);
    }

    RegularString (const String & from)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    RegularString (const char * from, size_t length)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from, length);
    }

    RegularString (const char * from)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    explicit
    RegularString (char from)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (&from, 1);
    }

    RegularString (const std::string & from)
        : String (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    RegularString(const std::string_view & from)
        : String(BuiltinCapacity<BUFFER_SIZE>())
    {
        assign(from);
    }


    RegularString & operator = (const char * from)
    {
        assign (from);

        return *this;
    }

    RegularString & operator = (const std::string & from)
    {
        assign (from);

        return *this;
    }
    RegularString & operator = (const std::string_view & from)
    {
        assign(from);

        return *this;
    }

    RegularString & operator = (const String & from)
    {
        assign (from);

        return *this;
    }

    RegularString & operator = (const RegularString & from)
    {
        assign (from);

        return *this;
    }

private:

    char _extension1 [BUFFER_SIZE - String::BUFFER_SIZE];
};

class MediumString : public RegularString
{
protected:

    template<std::uint16_t CAPACITY>
    MediumString (const BuiltinCapacity<CAPACITY> & capacity)
        : RegularString (capacity)
    {
    }

public:

    enum { BUFFER_SIZE = 256 };

    using TypeDescriptor = Cadre::Data::TypeDescriptor<MediumString, RegularString, ElementaryType, false, true>;

    MediumString ()
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
    }

    MediumString (const MediumString & value)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (value);
    }

    MediumString (const String & from)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    MediumString (const char * from, size_t length)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from, length);
    }

    MediumString (const char * from)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    explicit
    MediumString (char from)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (&from, 1);
    }

    MediumString (const std::string & from)
        : RegularString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    MediumString(const std::string_view & from)
        : RegularString(BuiltinCapacity<BUFFER_SIZE>())
    {
        assign(from);
    }

    MediumString & operator = (const char * from)
    {
        assign (from);

        return *this;
    }

    MediumString & operator = (const std::string & from)
    {
        assign (from);

        return *this;
    }
    MediumString & operator = (const std::string_view & from)
    {
        assign(from);

        return *this;
    }

    MediumString & operator = (const String & from)
    {
        assign (from);

        return *this;
    }

    MediumString & operator = (const MediumString & from)
    {
        assign (from);

        return *this;
    }

private:

    char _extension2 [BUFFER_SIZE - RegularString::BUFFER_SIZE];
};

class LongString : public MediumString
{
protected:

    template<std::uint16_t CAPACITY>
    LongString (const BuiltinCapacity<CAPACITY> & capacity)
        : MediumString (capacity)
    {
    }

public:

    enum { BUFFER_SIZE = CADRE_STRING_MAX_BUILTIN_CAPACITY };

    using TypeDescriptor = Cadre::Data::TypeDescriptor<LongString, MediumString, ElementaryType, false, true>;

    LongString ()
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
    }

    LongString (const LongString & value)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (value);
    }

    LongString (const String & from)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    LongString (const char * from, size_t length)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from, length);
    }

    LongString (const char * from)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }

    explicit
    LongString (char from)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (&from, 1);
    }

    LongString (const std::string & from)
        : MediumString (BuiltinCapacity<BUFFER_SIZE> ())
    {
        assign (from);
    }
    LongString(const std::string_view & from)
        : MediumString(BuiltinCapacity<BUFFER_SIZE>())
    {
        assign(from);
    }


    LongString & operator = (const char * from)
    {
        assign (from);
        
        return *this;
    }

    LongString & operator = (const String & from)
    {
        assign (from);

        return *this;
    }

    LongString & operator = (const std::string & from)
    {
        assign (from);

        return *this;
    }
    LongString & operator = (const std::string_view & from)
    {
        assign(from);

        return *this;
    }

    LongString & operator = (const LongString & from)
    {
        assign (from);

        return *this;
    }

private:

    char _extension3 [BUFFER_SIZE - MediumString::BUFFER_SIZE];
};

// String operators
template<typename T>
inline String &
operator += (String & left, const T & right)
{
    return left.append (right);
}

inline bool
operator == (const String & left, const String & right)
{
    return left.equal_to (right.c_str (), right.size ());
}

inline bool
operator == (const String & left, const char * right)
{
    return left.equal_to (right, std::strlen (right));
}

inline bool
operator == (const String & left, const std::string & right)
{
    return left.equal_to (right.c_str (), right.size ());
}

inline bool
operator == (const char * left, const String & right)
{
    return right.equal_to (left, std::strlen (left));
}

inline bool
operator == (const std::string & left, const String & right)
{
    return right.equal_to (left.c_str (), left.size ());
}
inline bool
operator == (const std::string_view & left, const String & right)
{
    return right.equal_to(left.data(), left.size());
}


inline bool
operator != (const String & left, const String & right)
{
    return ! (left == right);
}

inline bool
operator != (const String & left, const char * right)
{
    return ! (left == right);
}

inline bool
operator != (const String & left, const std::string & right)
{
    return ! (left == right);
}


inline bool
operator != (const char * left, const String & right)
{
    return ! (left == right);
}

inline bool
operator != (const std::string & left, const String & right)
{
    return ! (left == right);
}
inline bool
operator != (const std::string_view & left, const String & right)
{
    return !(left == right);
}


inline bool
operator < (const String & left, const String & right)
{
    return 0 > left.compare (right);
}

inline bool
operator < (const String & left, const char * right)
{
    return 0 > left.compare (right, std::strlen (right));
}

inline bool
operator < (const String & left, const std::string & right)
{
    return 0 > left.compare (right.c_str (), right.size ());
}

inline bool
operator < (const String & left, const std::string_view & right)
{
    return 0 > left.compare(right.data(), right.size());
}

inline bool
operator < (const char * left, const String & right)
{
    return 0 < right.compare (left, std::strlen (left));
}

inline bool
operator < (const std::string & left, const String & right)
{
    return 0 < right.compare (left.c_str (), left.size ());
}
inline bool
operator < (const std::string_view & left, const String & right)
{
    return 0 < right.compare(left.data(), left.size());
}

inline bool
operator > (const String & left, const String & right)
{
    return right < left;
}

inline bool
operator > (const String & left, const char * right)
{
    return right < left;
}

inline bool
operator > (const char * left, const String & right)
{
    return right < left;
}

inline bool
operator > (const std::string & left, const String & right)
{
    return right < left;
}
inline bool
operator > (const std::string_view & left, const String & right)
{
    return right < left;
}

inline bool
operator <= (const String & left, const String & right)
{
    return ! (left > right);
}

inline bool
operator <= (const String & left, const char * right)
{
    return ! (left > right);
}

inline bool
operator <= (const char * left, const String & right)
{
    return ! (left > right);
}

inline bool
operator <= (const std::string & left, const String & right)
{
    return ! (left > right);
}
inline bool
operator <= (const std::string_view & left, const String & right)
{
    return !(left > right);
}

inline bool
operator >= (const String & left, const String & right)
{
    return ! (left < right);
}

inline bool
operator >= (const String & left, const char * right)
{
    return ! (left < right);
}

inline bool
operator >= (const char * left, const String & right)
{
    return ! (left < right);
}

inline bool
operator >= (const std::string & left, const String & right)
{
    return ! (left < right);
}
inline bool
operator >= (const std::string_view & left, const String & right)
{
    return !(left < right);
}

inline MediumString
operator + (const String & left, const String & right)
{
    return MediumString (left).append (right);
}

inline MediumString
operator + (const String & left, const char * right)
{
    return MediumString (left).append (right);
}

inline MediumString
operator + (const String & left, const std::string & right)
{
    return MediumString (left).append (right);
}
inline MediumString
operator + (const String & left, const std::string_view & right)
{
    return MediumString(left).append(right);
}

inline MediumString
operator + (const char * left, const String & right)
{
    return MediumString (left).append (right);
}

inline MediumString
operator + (const std::string & left, const String & right)
{
    return MediumString (left).append (right);
}
inline MediumString
operator + (const std::string_view & left, const String & right)
{
    return MediumString(left).append(right);
}

class StringOutputStream : private std::streambuf
{
    StringOutputStream (const StringOutputStream & os); // undefined

    virtual std::streamsize xsputn (const char * sz, std::streamsize size)
    {
        _buffer.append (sz, size);

        return size;
    }

    virtual std::streambuf::int_type overflow (std::streambuf::int_type c)
    {
        if (c != std::streambuf::traits_type::eof ())
        {
            _buffer.append (c);
        }

        return c;
    }

public:

    StringOutputStream (String & buffer);

    virtual ~StringOutputStream ()
    {
    }

    template<typename T>
    StringOutputStream & operator<< (const T & value)
    {
        _os << value;

        return *this;
    }

    StringOutputStream & operator<< (std::ostream & (*functor) (std::ostream &))
    {
        _os << functor;

        return *this;
    }

    StringOutputStream & operator<< (std::ios_base & (*functor) (std::ios_base &))
    {
        _os << functor;

        return *this;
    }

    std::ostream & stream ()
    {
        return _os;
    }

    operator std::ostream & ()
    {
        return _os;
    }

private:

    String &     _buffer;
    std::ostream _os;
};



/*******************************************************************

  NOTE!!! 
  The following string_hash MUST match string_hash in Core.cst!!!
    
********************************************************************/
inline size_t
string_hash (boost::uint32_t seed, const char * value)
{
    //from boost
    const char * pos = value;
    boost::uint32_t hash (seed);
    for (; 0 != *pos; ++pos)
    {
        boost::uint32_t arg (hash >> 2);
        arg += (hash << 6);
        arg += 0x9e3779b9;
        arg += *pos;

        hash ^= arg;
    }
    
    return hash;
}

inline size_t
hash_value (const String & x)
{
    return string_hash (0, x.c_str ());
}


namespace stdext
{

template<typename T>
inline size_t
hash_value (const Cadre::Data::String & x)
{
    return Cadre::Data::hash_value (x);
}

} // namespace stdext

namespace std
{

/// Case insensitive sort functor
template<typename T>
struct nocase_less
{
    bool operator() (const T & s1, const T & s2) const 
    {         
        return boost::algorithm::lexicographical_compare
        (
            s1, s2, boost::algorithm::is_iless ()
        );
    } 
}; 

} // namespace std
