#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

using namespace std;
using namespace Catch::Matchers;

enum GuitarType : uint16_t; // forward declaration

void play(GuitarType g);

// definition of enum
enum GuitarType : uint16_t { strat = 1, tele, les_paul, };

void play(GuitarType g)
{
    if (g == strat)
    {
        std::cout << "play start\n";
    }
    else
    {
        std::cout << "play the guitar\n";
    }
}

TEST_CASE("enum types in C++98")
{
    GuitarType guitar = strat;

    std::underlying_type_t<GuitarType> value = guitar;

    auto value1 = guitar;

    guitar = static_cast<GuitarType>(2);

    REQUIRE(guitar == tele);
}

// scoped enumerations

enum class DayOfWeek : uint8_t { Mon = 1, Tue, Wed, Thd, Fri, Sat, Sun };

TEST_CASE("scoped enumeration")
{
    DayOfWeek day = DayOfWeek::Tue;

    auto value = static_cast<std::underlying_type_t<DayOfWeek>>(day);

    REQUIRE(value == 2);

    ++value;

    DayOfWeek next_day = static_cast<DayOfWeek>(value);

    REQUIRE(next_day == DayOfWeek::Wed);
}

TEST_CASE("Since C++17")
{
    //DayOfWeek day = 7;
    DayOfWeek day{7};

    REQUIRE(day == DayOfWeek::Sun);
}

TEST_CASE("byte in C++")
{
    SECTION("C++98")
    {
        unsigned char b1 = 0b00110011;

        unsigned char b2 = 0b11001100;

        unsigned char b3 = b1 | b2;

        unsigned char b4 = b1 + b2;

        std::cout << b2 << "\n";
    }

    SECTION("Since C++17")
    {
        std::byte b1{0b00110011};
        std::byte b2{0b11001100};

        std::byte b3 = b1 | b2;
        
        std::cout << std::to_integer<int>(b3) << "\n";
    }
}