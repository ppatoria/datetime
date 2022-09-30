#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

#include <dt/local_date_time.hpp>
#include <dt/utc_date_time.hpp>
#include <iostream>

TEST_CASE("test", "[test]")
{
    auto now_utc = dt::utc_date_time::now().toString();
    std::cout << now_utc << std::endl;

    auto now_local = dt::local_date_time::now().toString();
    std::cout << now_local << std::endl;
}
