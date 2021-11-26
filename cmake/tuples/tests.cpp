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

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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

    size_t hashed() const
    {
        size_t seed = 0;
        
        hash_combine(seed, fname);
        hash_combine(seed, lname);
        hash_combine(seed, age);

        // see below
        // tuple_apply([&](const auto& item)
        //     { hash_combine(seed, item) },
        //     tied());
        return seed;
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

///////////////////////////////////////
// index sequence

template <typename... TArgs>
struct Row
{
    std::tuple<TArgs...> data;

    template <size_t... Indices>
    auto select()
    {
        return std::tuple {std::get<Indices>(data)...};
    }
};

TEST_CASE("index sequence")
{
    Row<int, double, float, std::string> datarow {{1, 3.14, 2.71f, "abc"}};

    REQUIRE(datarow.select<0, 2, 3>() == std::tuple {1, 2.71f, "abc"s});
}

// ---------------------------------------------------------------------------------------------------------------------
// nested parameter packs
template <typename... Args1>
struct Zip
{
    template <typename... Args2>
    struct With
    {
        using type = tuple<pair<Args1, Args2>...>;
    };
};

TEST_CASE("zipping")
{
    Zip<int, double, char>::With<string, int, double>::type zipped;
    get<0>(zipped) = make_pair(1, "Hello");
    get<1>(zipped) = make_pair(3.14, 5);
    get<2>(zipped) = make_pair('s', 5.55);
}

// ---------------------------------------------------------------------------------------------------------------------
// tuple_apply - foreach for tuple

template <typename F, typename T, size_t... Is>
void tuple_apply_impl(F&& f, const T& t, std::index_sequence<Is...>)
{
    (..., f(std::get<Is>(t))); // fold expression - operator ,
}

template <typename F, typename... Ts>
void tuple_apply(F&& f, const std::tuple<Ts...>& t)
{
    using Indices = std::make_index_sequence<sizeof...(Ts)>;
    tuple_apply_impl(f, t, Indices {});
}

TEST_CASE("foreach for tuple")
{
    auto printer = [](const auto& item)
    { std::cout << "value: " << item << "\n"; };

    auto tpl = tuple {1, 3.14, "text"};

    tuple_apply(printer, tpl);
}
