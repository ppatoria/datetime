#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <functional>

namespace dt {
namespace string {
    inline auto to_double = std::bind(boost::lexical_cast<double, std::string>,
        std::placeholders::_1);

    inline auto to_lower_copy_of = std::bind(boost::algorithm::to_lower_copy<std::string>,
        std::placeholders::_1, std::locale());
}
}
