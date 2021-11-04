#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

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

int bar(std::string s)
{
    return 665;
}

TEST_CASE("decltype")
{
    std::map<int, std::string> dict1 = { {1, "one"}, {2, "two"}, {3, "three"} };

    auto dict2 = dict1;

    REQUIRE(dict2.size() == 3);

    decltype(dict1) dict3;

    REQUIRE(dict3.size() == 0);

    int tab[10];

    auto& ref_tab = tab; // int(&)[10]

    std::remove_reference_t<decltype(tab[0])> item; // int item;

    decltype(bar("some text"s)) bar_result1;
    decltype(bar(std::declval<std::string>())) bar_result2;
}

TEST_CASE("non-evaluated context")
{
    std::map<int, std::string> dict;

    REQUIRE(dict.size() == 0);

    REQUIRE(sizeof(decltype(dict[0])) == 28);

    REQUIRE(dict.size() == 0);
}

template <typename T>
auto full_name(const T& first, const T& last)
{
    if (first.empty())
        return last; 

    return first + last;
}

TEST_CASE("auto return type deduction for functions")
{
    std::string f = "Jan";
    std::string l = "Kowalski";

    auto fname = full_name(f, l);
}

template <typename T>
decltype(auto) get_value(T& dict, const std::string& key) // -> float&
{
    return dict.at(key);
}

template <typename T>
decltype(auto) get_value(T& container, size_t index)
{
    return container[index];
}

TEST_CASE("auto vs. decltype(auto)")
{
    std::map<std::string, float> math_vars = { {"pi", 3.14}, {"e", 2.72} };
    get_value(math_vars, "pi") = 3.1415f;

    std::vector<bool> vec = {1, 0, 1, 1};
    get_value(vec, 1) = 1;

    REQUIRE(vec[1] == true);
}