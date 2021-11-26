#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <array>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("constexpr")
{
    constexpr double pi = 3.1415;

    constexpr double two_pi = pi * 2.0;

    constexpr double e = 2.71;

    constexpr auto value = e * two_pi;
}

constexpr uint64_t factorial(uint64_t n)
{
    return (n == 0) ? 1 : n * factorial(n-1);
}

template <size_t N>
constexpr std::array<uint64_t, N> create_factorial_lookup()
{
    std::array<uint64_t, N> result{};

    for(size_t n = 0; n < N; ++n)
        result[n] = factorial(n);

    return result;
}
    
template <auto N>
struct Value
{
    static auto const value = N;
};

TEST_CASE("non-type params of template")
{
    static_assert(Value<4>::value == 4);
    //static_assert(Value<4.0>::value == 4);
}

constexpr int check(int i)
{
    constexpr int low = 0;
    constexpr int high = 99;
 
    return (low <= i && i < high) 
                ? i 
                : throw std::out_of_range("range error");
}

TEST_CASE("constexpr functions")
{
    std::array<int, factorial(4)> tab1{};

    constexpr auto factorial_lookup = create_factorial_lookup<10>();

    static_assert(create_factorial_lookup<10>()[4] == 24);

    constexpr int val1 = check(50);
    //constexpr int val2 = check(500);

    REQUIRE_THROWS_AS(check(500), std::out_of_range);
}

template <typename InpIter, typename Pred>
constexpr InpIter constexpr_find_if(InpIter first, InpIter last, Pred pred)
{
    for(auto it = first; it != last; ++it)
        if (pred(*it))
            return it;
    return last;
}

TEST_CASE("lambda are implicitly constexpr - since C++17")
{
    auto square = [](int x) { return x * x; };

    std::array<int, square(8)> tab1{};

    constexpr auto factorial_lookup = create_factorial_lookup<10>();

    constexpr auto first_gt_1000 = *constexpr_find_if(begin(factorial_lookup), end(factorial_lookup), [](int x) { return x > 1000; });
}