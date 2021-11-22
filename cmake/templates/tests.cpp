#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename T>
T maximum(T a, T b)
{
    puts(__PRETTY_FUNCTION__);
    return a < b ? b : a;
}

template <typename T>
T maximum(T* a, T* b)
{
    puts(__PRETTY_FUNCTION__);
    return *a < *b ? *b : *a;
}

const char* maximum(const char* a, const char* b)
{
    puts(__PRETTY_FUNCTION__);
    return strcmp(a, b) < 0 ? b : a;
}

TEST_CASE("function template")
{
    REQUIRE(maximum<int>(4, 6) == 6);

    REQUIRE(maximum(3.14, 2.71) == Approx(3.14));

    REQUIRE(maximum(static_cast<double>(3), 3.14) == Approx(3.14));

    const char* txt1 = "hbc";
    const char* txt2 = "def";

    REQUIRE(maximum(txt1, txt2) == "hbc"s);
}

//////////////////////////////////////////////////////////
// controlling types of returned values from function templates

namespace ver_1
{
    template <typename TResult, typename T1, typename T2>
    TResult add(const T1& a, const T2& b)
    {
        return a + b;
    }
}

namespace ver_2
{
    template <typename T1, typename T2>
    decltype(auto) add(const T1& a, const T2& b)
    {
        return a + b;
    }
}

namespace ver_3
{
    template <typename T1, typename T2>
    std::common_type_t<T1, T2> maximum(const T1& a, const T2& b)
    {
        return a < b ? b : a;
    }
}

namespace ver_4
{
    template <typename T1, typename T2, typename ResultT = std::common_type_t<T1, T2>>
    ResultT maximum(const T1& a, const T2& b)
    {
        return a < b ? b : a;
    }
}

TEST_CASE("controlling return type")
{
    REQUIRE(ver_1::add<int>(1, 3.14) == 4);

    REQUIRE(ver_2::add(1, 3.14) == Approx(4.14));

    REQUIRE(ver_3::maximum(1, 3.14) == Approx(3.14));

    REQUIRE(ver_3::maximum(uint8_t{65}, 300.14) == Approx(300.14));

    REQUIRE(ver_3::maximum("abc", "def"s) == "def"s);

    REQUIRE(ver_4::maximum<int, double, int>(1, 3.14) == 3);
}