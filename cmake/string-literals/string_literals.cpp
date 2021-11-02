#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Catch::Matchers;

TEST_CASE("raw-string literals")
{
    std::string path1 = "C:\\nasz katalog\\backup";
    std::string path2 = R"(C:\nasz katalog\backup)";

    std::cout << path1 << "\n" << path2 << "\n";

    std::string multiline_text = R"(line1
line2
line3)";

    std::cout << multiline_text << "\n";
}

TEST_CASE("custom delimiters")
{
    std::string text = R"insert(cytat: "(text)")insert";

    std::cout << text << "\n";
}

TEST_CASE("Since C++14 - string literals")
{
    auto ctext = "text"; // const char*

    auto text = "text"s; // std::string

    auto wtext = L"text"s; // std::wstring
}