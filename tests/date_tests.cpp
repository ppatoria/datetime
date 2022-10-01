#include "catch2/catch_all.hpp"
#include <dt/date.hpp>
#include <string>

TEST_CASE("date", "[misc tests]")
{
    dt::date date1;
    REQUIRE(date1.to_string() == std::string());

    dt::date date2(boost::gregorian::date { 2016, 3, 21 });
    REQUIRE(date2.to_string() == std::string("2016-03-21"));

    dt::date date3(dt::gregorian_date { 2017, 4, 24 });
    REQUIRE(date3.to_string() == std::string("2017-04-24"));

    dt::date date4(dt::date::year_type(2022), dt::date::month_type(9), dt::date::day_type(2));
    REQUIRE(date4.to_string() == std::string("2022-09-02"));

    dt::date date5(boost::gregorian::date::year_type(2022), boost::gregorian::date::month_type(9),
        boost::gregorian::date::day_type(2));
    REQUIRE(date5.to_string() == std::string("2022-09-02"));

    REQUIRE(date4 == date5);

    dt::date date6(2022, 9, 2);
    REQUIRE(date6.to_string() == std::string("2022-09-02"));

    date6.clear();

    REQUIRE(date6.empty() == true);

    REQUIRE(date5.to_string("%d/%m/%Y") == std::string("02/09/2022"));

    dt::date date7;
    date7.from_string("09-03-2020", "%d-%m-%Y");
    REQUIRE(date7.year() == 2020);
    REQUIRE(date7.month() == 3);
    REQUIRE(date7.day() == 9);
}
