#include <dt/LocalDateTime.hpp>
#include <dt/UtcDateTime.hpp>
#include <iostream>

int main()
{
    auto now_utc = dt::UtcDateTime::now().toString();
    std::cout << now_utc << std::endl;

    auto now_local = dt::local_date_time::now().toString();
    std::cout << now_local << std::endl;

    return 0;
}
