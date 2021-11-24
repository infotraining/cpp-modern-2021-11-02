#include "catch.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
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

TEST_CASE("using Array")
{
    Array<int, 5> arr1{1, 2, 3, 4, 5};

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
class TypeList {};

template <typename... Ts>
struct Row
{
    std::tuple<Ts...> data; // expansion pack
};

TEST_CASE("variadic templates")
{
    TypeList<int, double, void, std::string> tl1;
    TypeList<> tl2;

    Row<int, std::string, double> r1{{1,"text", 3.14}};
}
