#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

enum GuitarType { strat = 1, tele, les_paul, };

TEST_CASE("enum types in C++98")
{
    GuitarType guitar = strat;

    int value = guitar;

    guitar = static_cast<GuitarType>(2);

    REQUIRE(guitar == tele);
}