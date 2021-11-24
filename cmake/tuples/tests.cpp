#include "catch.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

template <typename TContainer>
std::tuple<int, int, double> calculate_stats(const TContainer& data)
{
    auto [min_pos, max_pos] = std::minmax_element(std::begin(data), std::end(data));
    auto avg = std::accumulate(std::begin(data), std::end(data), 0.0) / std::size(data);

    return std::tuple {*min_pos, *max_pos, avg};
}

TEST_CASE("using tuples")
{
    std::vector vec = {1, 2, 3, 4};

    SECTION("using get")
    {
        tuple<int, int, double> results = calculate_stats(vec);

        REQUIRE(std::get<0>(results) == 1);
        REQUIRE(std::get<1>(results) == 4);
    }

    SECTION("using get")
    {
        int min, max;
        double avg;

        std::tie(min, max, std::ignore) = calculate_stats(vec);

        SECTION("it works like this")
        {
            std::tuple<int&, int&, double&> ref_tpl {min, max, avg};
            ref_tpl = calculate_stats(vec);
        }

        REQUIRE(min == 1);
        REQUIRE(max == 4);
    }

    SECTION("using structured bindings - since C++17")
    {
        auto [min, max, avg] = calculate_stats(vec);

        REQUIRE(min == 1);
        REQUIRE(max == 4);

        std::set<int> numbers = {1, 3, 4, 5, 7};

        auto [pos, was_inserted] = numbers.insert(6);
        REQUIRE(was_inserted);
    }
}

template <typename T>
struct Comparable
{
protected:
    bool equals(const T& other) const
    {
        return static_cast<const T*>(this)->tied() == other.tied();
    }

    bool less(const T& other) const
    {
        return static_cast<const T*>(this)->tied() < other.tied();
    }
public:
    bool operator==(const T& other) const
    {
        return equals(other);
    }

    bool operator!=(const T& other) const
    {
        return !equals(other);
    }

    bool operator<(const T& other) const
    {
        return less(other);
    }

    bool operator>(const T& other) const
    {
        return !less(other) && !equals(other);
    }

    bool operator<=(const T& other) const
    {
        return less(other) || equals(other);
    }

    bool operator>=(const T& other) const
    {
        return !less(other);
    }
};

struct Person : Comparable<Person> // provides operators: ==, !=, <, >, <=, >=
{
    std::string fname;
    std::string lname;
    int age;

    Person(std::string fn, std::string ln, int age)
        : fname(std::move(fn))
        , lname(std::move(ln))
        , age(age)
    {
    }

    auto tied() const
    {
        return std::tie(lname, fname, age);
    }
};

TEST_CASE("using tuples to compare objects")
{
    Person p1 {"Jan", "Kowalski", 44};
    Person p2 {"Jan", "Kowalski", 45};
    Person p3 {"Jan", "Kowalski", 45};

    REQUIRE(p1 != p2);
    REQUIRE(p2 == p3);
    REQUIRE(p1 < p2);

    auto [first_name, last_name, age] = p1.tied();
}