#include "catch.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <list>
#include <array>

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

    REQUIRE(ver_3::maximum(uint8_t {65}, 300.14) == Approx(300.14));

    REQUIRE(ver_3::maximum("abc", "def"s) == "def"s);

    REQUIRE(ver_4::maximum<int, double, int>(1, 3.14) == 3);
}

//////////////////////////////////////////////////////////////////
// class templates

template <typename T>
class Holder
{
    T item_;

public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    Holder(const T& val)
        : item_(val)
    {
    }

    Holder(T&& val)
        : item_(std::move(val))
    {
    }

    reference value()
    {
        return item_;
    }

    const_reference value() const
    {
        return item_;
    }
};

////////////////////////////////////
// partial specialization Holder<T*>

template <typename T>
class Holder<T*>
{
    std::unique_ptr<T> item_;

public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;

    Holder(pointer&& val)
        : item_ {val}
    {
        if (!item_)
            throw std::invalid_argument("Pointer cannot be null");
    }

    Holder(std::unique_ptr<T> val)
        : item_ {std::move(val)}
    {
        if (!item_)
            throw std::invalid_argument("Pointer cannot be null");
    }

    reference value()
    {
        return *item_;
    }

    const_reference value() const
    {
        return *item_;
    }

    T* get() const
    {
        return item_;
    }
};

////////////////////////////////
// concrete specialization Holder<const char*>
template <>
class Holder<const char*>
{
    std::string item_;

public:
    using value_type = std::string;
    using reference = value_type&;
    using const_reference = const value_type&;

    Holder(std::string val)
        : item_ {std::move(val)}
    {
    }

    reference value()
    {
        return item_;
    }

    const_reference value() const
    {
        return item_;
    }
};

TEST_CASE("using Holder")
{
    Holder<int> h1 {42};

    REQUIRE(h1.value() == 42);
    static_assert(std::is_same_v<Holder<int>::value_type, int>);

    Holder<int*> h2 {new int(665)};
    REQUIRE(h2.value() == 665);

    int* a = new int(665);
    Holder<int*> h3 {std::move(a)};

    Holder<const char*> h4 {"text"};
    REQUIRE(h4.value() == "text"s);
}

template <typename T, size_t N>
struct Array
{
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;

    T items[N];

    iterator begin()
    {
        return items;
    }

    iterator end()
    {
        return items + N;
    }

    const_iterator begin() const
    {
        return items;
    }

    const_iterator end() const
    {
        return items + N;
    }

    reference operator[](size_t index)
    {
        return items[index];
    }

    const_reference operator[](size_t index) const
    {
        return items[index];
    }

    size_t size() const
    {
        return N;
    }
};

template<typename THead, typename... TArgs>
Array(THead, TArgs...) -> Array<THead, sizeof...(TArgs) + 1>;

TEST_CASE("using Array")
{
    Array arr1 {1, 2, 3, 4, 5};

    REQUIRE(arr1.size() == 5);

    for (const auto& item : arr1)
        std::cout << item << " ";
    std::cout << "\n";
}

template <
    typename T,
    template <typename, typename> class Container,
    typename TAllocator = std::allocator<T>>
class Stack
{
    Container<T, TAllocator> items_;

public:
    Stack() = default;

    void push(const T& item)
    {
        items_.push_back(item);
    }

    const T& top() const
    {
        return items_.back();
    }

    void pop()
    {
        items_.pop_back();
    }
};

TEST_CASE("using Stack")
{
    Stack<int, std::vector> s1;

    s1.push(1);
    s1.push(42);

    REQUIRE(s1.top() == 42);
}

///////////////////////////////////////////////////////////
// type traits

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

////////////////////////////////
// IsVoid

template <typename T>
struct IsVoid
{
    static const bool value = false;
};

template <>
struct IsVoid<void>
{
    static const bool value = true;
};

template <typename T>
constexpr bool IsVoid_v = IsVoid<T>::value;

template <typename T>
void use(T* ptr)
{
    static_assert(!IsVoid_v<T>, "Void as a pointer type is unacceptable");
    static_assert(!std::is_void_v<T>, "Void as a pointer type is unacceptable");
}

TEST_CASE("type traits")
{
    static_assert(std::is_same_v<Identity<int>::type, int>);
    static_assert(std::is_same_v<Identity_t<int>, int>);

    int* ptr;

    use(ptr);
}

/////////////////////////////
// custom RemoveReference

template <typename T>
struct RemoveReference
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&>
{
    using type = T;
};

template <typename T>
struct RemoveReference<T&&>
{
    using type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::type;

namespace Traits
{
    template <typename TContainer>
    auto sum(const TContainer& collection)
    {
        using TResult = std::remove_cv_t<RemoveReference_t<decltype(*begin(collection))>>;

        TResult value {};
        for (const auto& element : collection)
        {
            value += element;
        }
        return value;
    }
}

TEST_CASE("sum")
{
    const int vec[] = {1, 2, 3};
    REQUIRE(Traits::sum(vec) == 6);
}

////////////////////////////////////////////////////////////////////////
// variadic templates

template <typename... Ts>
class TypeList
{
};

template <typename... Ts>
struct Row
{
    std::tuple<Ts...> data; // expansion pack
};

TEST_CASE("variadic templates")
{
    TypeList<int, double, void, std::string> tl1;
    TypeList<> tl2;

    Row<int, std::string, double> r1 {{1, "text", 3.14}};
}

void print()
{
    std::cout << "\n";
}

template <typename THead, typename... TTail>
void print(const THead& head, const TTail&... tail)
{
    std::cout << head << " ";
    print(tail...);
}

namespace SinceCpp17
{
    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";

        if constexpr(sizeof...(tail) > 0)
            print(tail...);
        else
            std::cout << "\n";
    }
}

TEST_CASE("head-tail print")
{
    print(1, 3.14, "text"s, "abc");
    SinceCpp17::print("pi", 3.14);
}

template <typename T>
auto sum(const T& value)
{
    return value;
}

 template <typename THead, typename... TTail>
 auto sum(const THead& head, const TTail&... tail)
 {
     return head + sum(tail...);
 }

namespace FoldExpressions
{
    template <typename... TArgs>
    struct TypesAreTheSame;


    template <typename THead, typename... TTail>
    struct TypesAreTheSame<THead, TTail...>
    {
        constexpr static bool value = (... && std::is_same_v<THead, TTail>);
    };

    template <typename... TArgs>
    constexpr static bool TypesAreTheSame_v = TypesAreTheSame<TArgs...>::value;

    template <typename... TArgs>
    auto sum(const TArgs&...  args)  // sum(1, 2, 3, 4)
    {
        static_assert(TypesAreTheSame_v<TArgs...>);

        return (... + args); // left-fold -> (((1 + 2) + 3 ) + 4 )
    }

    template <typename... TArgs>
    auto sum_right(const TArgs&...  args)  // sum(1, 2, 3, 4)
    {
        return (args + ...); // right-fold -> (((4 + 3) + 2 ) + 1)
    }

    template <typename F, typename... TArgs>
    auto map_reduce(F&& f, const TArgs&...  args)  // map_reduce(f, 1, 2, 3, 4)
    {
        return (... + f(args)); // left-fold -> (((f(1) + f(2)) + f(3)) + f(4) )
    }

    template <typename... TArgs>
    void print(const TArgs&... args)
    {
        bool first_character = true;
        auto with_space = [&first_character](const auto& item) {
            if (!first_character)
                std::cout << " ";
            else
                first_character = false;
            return item; 
        };

        (std::cout << ... << with_space(args)) << "\n";
    }

    template <typename... TArgs>
    bool all_true(TArgs... args)
    {
        return (... && args);
    }

    template <typename... TArgs>
    void print_lines(const TArgs&... args)
    {
        (..., (std::cout << args << "\n")); // comma-operator with left-fold expression
    }
}

TEST_CASE("head-tail sum")
{
    REQUIRE(FoldExpressions::sum(1, 2, 3, 4, 5) == 15);

    REQUIRE(FoldExpressions::map_reduce([](int x) { return x * x; }, 1, 2, 3) == 14);

    FoldExpressions::print(1, 3.14, "text");

    REQUIRE(FoldExpressions::all_true(true, true, 1));
    REQUIRE(FoldExpressions::all_true());

    FoldExpressions::print_lines(1, 2, "abc", "def"s);
}

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair(const T1& f, const T2& s) : fst{f}, snd{s}
    {}
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

/////////////////////////////////////////////////////////////////////////
// CTAD - class template argument deduction

TEST_CASE("CTAD")
{
    std::pair<int, double> p1{1, 3.14};
    std::pair<int, std::string> p2{4, "four"s};
    auto p3 = std::make_pair(6, "six"s);
    auto p4 = std::pair(6, "six"s);

    ValuePair<int, std::string> vp1{1, "one"s};
    ValuePair vp2{7, "seven"s}; // CTAD

    int tab[5] = {1, 2, 3, 4, 5};
    ValuePair vp3{"array", tab}; // ValuePair<const char*, int*>

    const int x1 = 10;
    double d1 = 3.14;

    const int& rx1 = x1;
    double& rd2 = d1;

    ValuePair vp4{rx1, rd2}; // ValuePair<int, double>

    std::vector vec{1, 2, 3, 4}; // CTAD

    std::vector other_vec{vec}; // std::vector<int>
    std::vector another_vec{vec, vec}; // std::vector<std::vector<int>>

    std::tuple tp1{1, "one", 3.14}; // std::tuple<int, cons char*, double>
    std::tuple tp2{tp1}; // CTAD special copy case - std::tuple<int, cons char*, double>
}

template <typename T1, typename T2>
struct Aggregate
{
    T1 item1;
    T2 item2;
};

template <typename T1, typename T2>
Aggregate(T1, T2) -> Aggregate<T1, T2>;

TEST_CASE("CTAD for aggregates")
{
    Aggregate agg1{1, 3.14};
}

int foo(int x) { return x * 42; }

TEST_CASE("CTAD in std library")
{
    std::pair p1{1, "one"s}; // std::pair<int, std::string>

    std::tuple t1{1, "one", 3.14}; // std::tuple<int, const char*, double>

    std::optional i1 = 42; // std::optional<int>
    std::optional i2(i1); // std::optional<int>

    auto sp = std::make_shared<std::string>("text");
    std::weak_ptr wp = sp;

    std::function f = foo;

    std::vector vec = {1, 2, 3};
    REQUIRE(vec == std::vector{1, 2, 3});

    std::list lst(begin(vec), end(vec));

    std::array arr = {1, 2, 3, 4, 5, 6};
}