#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <optional>
#include <atomic>
#include <charconv>
#include <array>
#include <any>
#include <map>
#include <variant>

using namespace std;

TEST_CASE("string_view")
{
    const char* ctext = "abc def";
    std::string text = "ghi jkl";

    string_view sv1 = ctext;
    REQUIRE(sv1.size() == 7);

    string_view sv2 = text;

    string_view token1(ctext, 3);
    REQUIRE(token1 == "abc"sv);

    string_view token2(text.c_str(), 3);
    REQUIRE(token2 == "ghi"sv);

    SECTION("differences with string")
    {
        string_view sv;
        REQUIRE(sv.data() == nullptr);

        string s;
        REQUIRE(s.data() != nullptr);

        const char word[3] = {'o', 'n', 'e'};
        string_view sv_word{word, 3};

        std::cout << sv_word << "\n";
        REQUIRE(sv_word == "one");
    }
}

template <typename TContainer>
void print(std::string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for(const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

TEST_CASE("using string_view")
{
    vector vec{1, 2, 3};
    print("vector", vec);

    std::string prefix = "list";
    print(prefix, vector{6, 7, 8});

    std::string text = "text1 text2 text3";
    std::string_view sv_text = text;

    std::string backup(sv_text);
}

string_view start_from_word(string_view text, string_view word)
{
    return text.substr(text.find(word));
}

TEST_CASE("start from word")
{
    std::string text = "Ala ma kota";

    std::string_view result = start_from_word(text, "ma");
    std::cout << result << "\n";

    SECTION("beware of dangling pointer")
    {
        //std::string_view result = start_from_word("Ale mamy problem"s, "ma"); // temporary string causes returning dangling string_view
        
        //std::string_view evil = "evil"s; // dangling pointer
    }
}

TEST_CASE("using string_view to sort vector<string>")
{
    const std::vector<std::string> words = { "one", "two", "three", "four", "five", "six" };

    std::vector<std::string_view> words_sorted(begin(words), end(words));
    sort(begin(words_sorted), end(words_sorted));

    print("sorted words", words_sorted);
}

///////////////////////////////////////////////////////////
// optional

TEST_CASE("optional")
{
    std::optional<int> o1 = std::nullopt;

    REQUIRE(o1.has_value() == false);

    o1 = 42;

    REQUIRE(o1.has_value() == true);

    o1 = std::nullopt;

    REQUIRE(o1.has_value() == false);

    std::optional<std::atomic<int>> opt_atomic{std::in_place, 42};

    REQUIRE(opt_atomic.has_value() == true);

    std::optional name = "Jan"s; // std::optional<std::string> 

    if (name)
        std::cout << "Name: " << *name << "\n";

    REQUIRE(name.value() == "Jan"s);
    
    name.reset();

    REQUIRE_THROWS_AS(name.value(), std::bad_optional_access);

    REQUIRE(name.value_or("Not-Set") == "Not-Set"s);

    name = "Ambroży";

    optional target = std::move(name);

    REQUIRE(target.value() == "Ambroży"s);
    REQUIRE(name.has_value() == true);
}

optional<int> to_int(std::string_view str)
{
    int value{};

    auto start = str.data();
    auto end = str.data() + str.size();

    const auto [pos_end, error_code] = std::from_chars(start, end, value);

    if (error_code != std::errc{} || pos_end != end)
        return nullopt;
    
    return value;
}

TEST_CASE("optional - use case")
{
    optional<int> number = to_int("42");

    REQUIRE(number == 42);

    number = to_int("42ghjf"s);

    REQUIRE(number.has_value() == false);
}


template <typename TContainer>
constexpr std::optional<std::string_view> find_id(const TContainer& container, std::string_view id)
{
    for(const auto& item : container)
        if (item == id)
            return item;

    return nullopt;
}

TEST_CASE("string_view + optional + constexpr")
{
    constexpr std::array ids = { "one"sv, "two"sv, "three"sv };

    static_assert(ids[1] == "two"sv);

    constexpr optional opt_id = find_id(ids, "two"sv);

    static_assert(opt_id.has_value());
}

TEST_CASE("optional & reference")
{
    int x = 10;

    //std::optional<std::reference_wrapper<int>> opt_ref{x};
    std::optional opt_ref = std::ref(x);

    opt_ref.value().get() = 20;

    REQUIRE(x == 20);
}

/////////////////////////////////////////////////////////////////
// any

class Dictionary
{
    using MapType = std::map<std::string, std::any>;
    using PairType = MapType::value_type;
public:
    Dictionary(std::initializer_list<PairType> _items) : items(_items)
    {
    }

    template<typename T>
    T get(std::string key)
    {
        return std::any_cast<T>(items.at(key));
    }

private:
    std::map<std::string, std::any> items;
};

TEST_CASE("any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 42;
    anything = "string"s;
    anything = 3.14;
    anything = std::vector{1, 2, 3};

    std::vector<int> vec = std::any_cast<std::vector<int>>(anything);
    REQUIRE(vec == std::vector{1, 2, 3});

    REQUIRE_THROWS_AS(std::any_cast<double>(anything), std::bad_any_cast);

    anything = 665;

    int* ptr_value = std::any_cast<int>(&anything);

    if (ptr_value)
        std::cout << *ptr_value << "\n";


    SECTION("use-case with map")
    {
        Dictionary dict = { {"address"s, "Miodowa 33"s}, {"age", 75}, {"data", std::vector{1, 2, 3} } };

        REQUIRE(dict.get<std::string>("address") == "Miodowa 33"s);
        REQUIRE(dict.get<int>("age") == 75);
    }
}

//////////////////////////////////////////////////////////
// variant

class PrintVisitor
{
public:
    void operator()(int x) const 
    {
        std::cout << "int: " << x << "\n";
    }

    void operator()(const std::string& s) const
    {
        std::cout << "string: " << s << "\n";
    }

    void operator()(double x) const 
    {
        std::cout << "double: " << x << "\n";
    }

    void operator()(const std::vector<int>& v) const 
    {
        std::cout << "vector: ";
        for(const auto& item : v)
            std::cout << item << " ";
        std::cout << "\n";
    }
};

TEST_CASE("variant")
{
    std::variant<int, std::string, double, std::vector<int>> v1;

    REQUIRE(std::holds_alternative<int>(v1) == true);

    v1 = 3.14;
    REQUIRE(v1.index() == 2);
    v1 = "text"s;
    v1 = std::vector{1, 2, 3};
    v1 = 665;

    auto& item = std::get<int>(v1);
    REQUIRE(item == 665);    

    REQUIRE_THROWS_AS(std::get<double>(v1), std::bad_variant_access);

    int* ptr_value = std::get_if<int>(&v1);

    if (ptr_value)
        *ptr_value = 898;

    v1 = std::vector{1, 2, 3};

    std::visit(PrintVisitor{}, v1);
}

struct Square
{
    int size;
};

struct Rectangle
{
    int width, height;
};

struct Circle
{
    int radius;
};

struct Triangle
{

};

class AreaVisitor
{
public:
    double operator()(const Rectangle& rec)
    {
        return rec.height * rec.width;
    }

    double operator()(const Circle& circ)
    {
        return 3.14 * circ.radius * circ.radius;
    }

    double operator()(const Square& sq)
    {
        return sq.size * sq.size;
    }
};

template <typename... TClosures>
struct overloaded : TClosures...
{
    using TClosures::operator()...;
};

template <typename... TClosures>
overloaded(TClosures...) -> overloaded<TClosures...>;

TEST_CASE("polymorphism with std::variant")
{
    using Shape = std::variant<Rectangle, Circle, Square>;

    std::vector<Shape> shapes;

    shapes.push_back(Rectangle{10, 20});
    shapes.push_back(Circle{10});
    shapes.push_back(Square{40});

    double area{};

    auto area_visitor = overloaded {
        [](const Rectangle& rec) -> double
        {
            return rec.height * rec.width;
        },
        [](const Circle& circ) -> double
        {
            return 3.14 * circ.radius * circ.radius;
        },
        [](const Square& sq) -> double
        {
            return sq.size * sq.size;
        }
    };

    for(const auto& s : shapes)
        area += std::visit(area_visitor, s);

    std::cout << "Area: " << area << "\n";
}

[[nodiscard]] std::variant<std::string, std::errc> load_from_file(const std::string& filename)
{
    if (filename == "evil")
        return std::errc{13};
    return "content"s;
}

TEST_CASE("load_from_file")
{
    auto result = load_from_file("evil");

    std::visit(overloaded{ 
        [](const std::string& s) { std::cout << s << "\n"; },
        [](std::errc ec) { std::cout << "Error: " << static_cast<int>(ec) << "\n";}
    }, result);
}