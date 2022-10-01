#include "catch2/catch_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include <dt/time_duration.hpp>

TEST_CASE("time_duration", "[misc]")
{
    dt::time_duration td1(boost::posix_time::time_duration(1, 2, 3, 4));
    REQUIRE(td1.to_string() == std::string("01:02:03.000004"));

    dt::time_duration td2(1, 2, 3, 4);
    REQUIRE(td2.to_string() == std::string("01:02:03.000004"));

    REQUIRE(td1 == td2);

    dt::time_duration td3;
    REQUIRE(td3.empty() == true);
    td3 = td2;
    REQUIRE(td3 == td1);

    td1.clear();
    REQUIRE(td1.empty() == true);

    dt::time_duration td4(0, 0, 1);
    REQUIRE(td4.milliseconds() == 1000);

    dt::time_duration td5(0, 0, 0, 1);
    REQUIRE(td5.microseconds() == 1);
}
