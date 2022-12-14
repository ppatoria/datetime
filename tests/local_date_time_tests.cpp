#include "catch2/catch_all.hpp"
#include "catch2/catch_test_macros.hpp"
#include "dt/utc_date_time.hpp"
#include <dt/local_date_time.hpp>
#include <regex>

TEST_CASE("local_date_time", "[misc]")
{
    std::regex date_format("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{6}");

    auto utc = dt::utc_date_time::now();
    auto utc_date_str = utc.to_string();
    std::cout << utc_date_str << std::endl;
    REQUIRE(std::regex_match(utc.to_string(), date_format));

    dt::local_date_time local(utc);
    auto local_date_str = local.to_string();
    std::cout << local_date_str << std::endl;
    REQUIRE(std::regex_match(local_date_str, date_format));

    if (local.is_dst()) {
      REQUIRE(local.time_zone_name() == "EDT");
      REQUIRE(local.time_zone_offset() == "-04:00");
    } else {
      REQUIRE(local.time_zone_name() == "EST");
      REQUIRE(local.time_zone_offset() == "-05:00");
    }

    REQUIRE(utc.date() >= local.date());
    REQUIRE(utc.time() != local.time());
}
