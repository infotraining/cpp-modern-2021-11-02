#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <optional>
#include <atomic>
#include <charconv>
#include <array>

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