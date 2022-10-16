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

    std::cout << "is dst: " << local.is_dst() << std::endl;

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());
    std::cout << utc.time() - local.time() << std::endl;
    std::cout << local.time_zone_name() << std::endl;
    std::cout << local.time_zone_offset() << std::endl;
    std::cout << "local: " << local.time() << std::endl;
    REQUIRE(local.tz_difference_from_utc() == 5 * 60 * 60);
}
