#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>

namespace dt {
template <typename T, typename Enable = void>
struct TypeTraits {
    typedef T RegisteredType;
    typedef boost::true_type IsObject;

    BOOST_STATIC_ASSERT((
        !boost::is_fundamental<T>::value && !boost::is_reference<T>::value && !boost::is_pointer<T>::value && !boost::is_const<T>::value));

    // static const std::string toString (const boost::shared_ptr<T> & value)
    // {
    //   if (! value.get ())
    //     {
    //       return typeName<T> () + "<empty>";
    //     }

    //   return toString (*value);
    // }
};
} // namespace dt
