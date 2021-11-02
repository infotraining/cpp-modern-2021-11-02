#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("range-based-for")
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("iterating over container in C++98")
    {
        for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
        {
            (*it) *= 2;
        }

        for (std::vector<int>::const_iterator it = vec.begin(); it != vec.end(); ++it)
        {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    }

    SECTION("since C++11")
    {
        for (int item : vec)
        {
            std::cout << item << " ";
        }
        std::cout << "\n";

        SECTION("is interpreted as")
        {
            for (auto it = vec.begin(); it != vec.end(); ++it)
            {
                int item = *it;

                std::cout << item << " ";
            }
        }
    }

    SECTION("iteration over c-style array")
    {
        int tab[5] = {1, 2, 3, 4, 5};

        for (int item : tab)
        {
            std::cout << item << " ";
        }
        std::cout << "\n";

        SECTION("is interpreted as")
        {
            for (auto it = begin(tab); it != end(tab); ++it)
            {
                int item = *it;

                std::cout << item << " ";
            }
        }
    }

    SECTION("initializer lists")
    {
        for (const auto& item : {"one"s, "two"s, "three"s})
        {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
}

TEST_CASE("efficient range-based-for")
{
    std::vector<std::string> words = {"one", "two", "three"};

    for (const auto& item : words)
    {
        std::cout << item << "\n";
    }

    SECTION("is interpreted as")
    {
        for (auto it = words.begin(); it != words.end(); ++it)
        {
            const auto& item = *it; // cc

            std::cout << item << " ";
        }
        std::cout << "\n";
    }
}

struct Point
{
    int x, y;
};

struct Triangle
{
    Point points_[3];

    using iterator = Point*;
    using const_iterator = const Point*;

    iterator begin() { return points_; }
    const_iterator begin() const { return points_; }
    iterator end() { return points_ + 3; }
    const_iterator end() const { return points_ + 3; }

    // rest of implementation
};

TEST_CASE("range-based-for for custom type")
{
    Triangle t1{ { {1, 2},{2, 3}, {4,  5} } };

    for(const Point& pt : t1)
    {
        std::cout << "(" << pt.x << ", " << pt.y << ")\n";
    }
}