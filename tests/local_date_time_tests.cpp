#include "catch2/catch_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "dt/utc_date_time.hpp"
#include <dt/local_date_time.hpp>

TEST_CASE("local_date_time", "[misc]")
{
    /** TODO remove print staements */
    auto utc = dt::utc_date_time::now();
    std::cout << "utc: " << utc << std::endl;

    dt::local_date_time local(utc);
    std::cout << "local: " << local << std::endl;

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());
    std::cout << utc.time() - local.time() << std::endl;
    auto tzname = local.tz_name();
    std::cout << tzname.standard_time_zone << std::endl;
    if (local.is_daylight_saving()) {
        std::cout << tzname.daylight_time_zone << std::endl;
    }
    std::cout << "local: " << local << " " << tzname.daylight_time_zone
              << std::endl;
    std::cout << "local: " << local.time() << " "
              << tzname.daylight_time_zone << std::endl;
    REQUIRE(local.tz_difference_from_utc() == 5 * 60 * 60);
}
