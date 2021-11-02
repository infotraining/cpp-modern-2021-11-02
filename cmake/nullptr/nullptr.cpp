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

void foo(nullptr_t)
{
    std::cout << "foo(nullptr_t)\n";
}

// TEST_CASE("NULL is imperfect null pointer")
// {
//     foo(NULL);
// }

TEST_CASE("nullptr is better NULL")
{
    foo(nullptr);

    int* ptr1 = nullptr;
    int* ptr2{}; // universal init syntax - since C++11

    foo(ptr1);

    REQUIRE(ptr1 == nullptr);

    if (ptr2 != nullptr)
    {
        std::cout << *ptr2 << "\n";
    }
}
