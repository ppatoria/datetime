#include "catch2/catch_all.hpp"
#include <dt/date.hpp>

TEST_CASE("date", "[constructors]")
{
    dt::date date;
    REQUIRE(date.to_string() == std::string());
}
