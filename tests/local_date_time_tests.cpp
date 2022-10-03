#include "catch2/catch_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "dt/utc_date_time.hpp"
#include <dt/local_date_time.hpp>

TEST_CASE("local_date_time", "[misc]")
{

    auto utc = dt::utc_date_time::now();
    std::cout << "utc: " << utc << std::endl;

    dt::local_date_time local(utc);
    std::cout << "local: " << local << std::endl;

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());
}
