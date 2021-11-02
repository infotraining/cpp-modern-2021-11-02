#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;
using namespace Catch::Matchers;

int foo(int arg) 
{
    return arg;
}

TEST_CASE("auto {}")
{
    int x1 = 42;
    int x2(42);
    int x3{42};

    auto ax1 = 42;
    auto ax2(42);
    auto ax3{42};
}

TEST_CASE("auto declarations")
{
    auto i = 42; // int
    auto x(42);  // int
    auto y{42};  // int
    const auto d = 3.14;
    auto text = "text"s;

    std::set<int> numbers = {1, 2, 3, 4, 5};

    auto it = numbers.begin();
    auto& item = *it;

    for(auto it = numbers.begin(); it != numbers.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

template <typename T>
void deduce(T arg)
{
    puts(__FUNCSIG__);
}

// Since C++20
// void deduce(auto arg)
// {
//     puts(__FUNCSIG__);
// }

TEST_CASE("auto type deduction - case 1")
{
    int x = 10;
    auto ax1 = x; // int
    deduce(x);

    const int cx = 42;
    auto ax2 = cx; // int
    deduce(cx);

    int& ref_x = x;
    auto ax3 = ref_x; // int
    deduce(ref_x);

    const int& cref_x = x;
    auto ax4 = cref_x; // int
    deduce(cref_x);

    int tab[10];
    auto ax5 = tab; // int*
    deduce(tab);

    auto ax6 = foo; // int(*)(int)
    deduce(foo);
}

TEST_CASE("auto type deduction - case 2")
{
    int x = 10;
    auto& ar1 = x; // int&

    const int cx = 42;
    auto& ar2 = cx; // const int&

    int& ref_x = x;
    auto& ar3 = ref_x; // int&

    const int& cref_x = x;
    auto& ar4 = cref_x; // const int&

    int tab[10];
    auto& ar5 = tab; // int(&)[10]

    auto& ar6 = foo; // int(&)()
}