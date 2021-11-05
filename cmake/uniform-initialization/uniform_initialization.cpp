#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;
using namespace Catch::Matchers;

struct Data
{
    char c;
    double d;
    int data[5];
};

class Point
{
    int16_t x_;
    int16_t y_;

public:
    Point(int16_t x, int16_t y) : x_{x}, y_{y}
    {}

    int x() const
    {
        return x_;
    }

    int y() const
    {
        return y_;
    }
};

TEST_CASE("initialization syntax in C++98")
{
    SECTION("simple types")
    {
        int x = 10;
        int y(10);
        REQUIRE(x == y);

        // int z();
        // REQUIRE(z == 0);
    }

    SECTION("aggregates - c-like structures")
    {
        Data d1{ 'a', 3.14, {1, 2, 3} };
        Data d2{ 'a' };
        Data d3{};
    }

    SECTION("native arrays")
    {
        int tab1[4] = { 1, 2, 3, 4 };
        int tab2[4] = { 1, 2 };
        int tab3[4] = {};
    }

    SECTION("classes")
    {
        Point pt1(1, 2);

        REQUIRE(pt1.x() == 1);
        REQUIRE(pt1.y() == 2);
    }

    SECTION("containers")
    {
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
    }
}

int64_t foo()
{
    return 42;
}

TEST_CASE("initialization syntax in modern C++")
{
    SECTION("narrowing conversion")
    {
        int32_t value{static_cast<int32_t>(foo())};
    }

    SECTION("simple types")
    {
        int x = 10;
        int y{10};
        REQUIRE(x == y);

        int z{};
        REQUIRE(z == 0);
        
        int* ptr{};
        REQUIRE(ptr == nullptr);
    }

    SECTION("aggregates - c-like structures")
    {
        Data d1{ 'a', 3.14, {1, 2, 3} };
        Data d2{ 'a' };
        Data d3{};
    }

    SECTION("native arrays")
    {
        int tab1[4] = { 1, 2, 3, 4 };
        int tab2[4] = { 1, 2 };
        int tab3[4] = {};
    }

    SECTION("classes")
    {
        Point pt1{1, 2};

        REQUIRE(pt1.x() == 1);
        REQUIRE(pt1.y() == 2);
    }

    SECTION("containers")
    {
        const std::vector<int> vec{ 1, 2 };

        std::list<int> lst{1, 2, 3, 4};        
    }
}

TEST_CASE("beware - containers have constructors with initializer_list")
{
    std::vector<int> vec1{5, 2}; // std::vector<int>(std::initializer_list<int>)
    REQUIRE(vec1[0] == 5);
    REQUIRE(vec1[1] == 2);

    std::vector<int> vec2(5, 2); // std::vector<int>(size_t size, int value)
    REQUIRE(vec2.size() == 5);
    REQUIRE(vec2 == std::vector<int>{2, 2, 2, 2, 2});

    std::vector<std::string> words{2, "text"s};
    REQUIRE(words == std::vector{"text"s, "text"s});

    std::vector<int> empty{};
}