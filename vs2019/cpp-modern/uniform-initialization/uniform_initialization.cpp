#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("initialization syntax in C++98")
{
    SECTION("simple types")
    {
        int x = 10;
        int y(10);

        REQUIRE(x == y);

        int c();

        REQUIRE(c == 0);
    }
}