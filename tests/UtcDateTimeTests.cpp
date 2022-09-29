#include <dt/LocalDateTime.hpp>
#include <dt/UtcDateTime.hpp>
#include <iostream>

int main()
{
    using namespace dt;
    auto now_utc = UtcDateTime::now().toString();
    std::cout << now_utc << std::endl;

    auto now_local = LocalDateTime::now().toString();
    std::cout << now_local << std::endl;

    return 0;
}
