#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

void foo(int* ptr)
{
    std::cout << "foo(int*)\n";
}

void foo(int)
{
    std::cout << "foo(int)\n";
}

TEST_CASE("NULL is imperfect null pointer")
{
    foo(NULL);
}