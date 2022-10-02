#pragma once
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <functional>

namespace dt {
namespace string_util {

    template <typename T>
    inline T to(const std::string& str)
    {
        return boost::lexical_cast<T>(str);
    }

    inline std::string to_lower_copy(const std::string& str)
    {
        return boost::algorithm::to_lower_copy(str);
    }
} // namespace string_util
} // namespace dt
