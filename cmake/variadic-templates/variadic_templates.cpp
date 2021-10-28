#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;
using namespace Catch::Matchers;

// TEST_CASE("variadic templates")
// {
//     print(1, 3.14, "abc", "def"s);
// }

template <typename IndexSequence_>
struct IndexSequenceTail;

template <size_t Head_, size_t... Tail_>
struct IndexSequenceTail<std::index_sequence<Head_, Tail_...>>
{
    using type = std::index_sequence<Tail_...>;
};

TEST_CASE("index sequence tail test")
{
    using s = std::make_index_sequence<4>;
    using tail = IndexSequenceTail<s>::type;
    
}

template <typename Tuple_>
auto head(const Tuple_& tpl)
{
    return std::get<0>(tpl);
}

template <typename Tuple, size_t... Indexes_>
auto get_tuple_tail(const Tuple& tpl, std::index_sequence<Indexes_...>)
{
    return std::make_tuple(std::get<Indexes_>(tpl)...);
}

template<typename... Ts_>
auto tail(const std::tuple<Ts_...>& tpl)
{
    using tail_indexes = typename IndexSequenceTail<std::make_index_sequence<sizeof...(Ts_)>>::type;
    
    return get_tuple_tail(tpl, tail_indexes{});
}

TEST_CASE("head-tail")
{
    tuple<int, double, string> tpl{1, 3.14, "text"s};

    auto h = head(tpl);
    REQUIRE(h == 1);


    auto t = tail(tpl);

    static_assert(is_same<decltype(t), std::tuple<double, string>>::value, "Error");
    REQUIRE(t == std::tuple<double, string>(3.14, "text"s));
}