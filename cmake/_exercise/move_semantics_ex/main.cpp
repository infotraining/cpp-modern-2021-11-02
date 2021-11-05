#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <iostream>
#include "paragraph.hpp"

using namespace std;

TEST_CASE("Moving paragraph")
{
    LegacyCode::Paragraph p("***");
    LegacyCode::Paragraph mp = move(p);

    REQUIRE(mp.get_paragraph() == string("***"));
    REQUIRE(p.get_paragraph() == nullptr);
}

TEST_CASE("Moving text shape")
{
    Text txt{10, 20, "text"};
    Text mtxt = move(txt);

    REQUIRE(mtxt.text() == string("text"));
    REQUIRE(txt.text() == string());
}

TEST_CASE("swap for Text")
{
    Text t1{1, 2, "AAA"};
    Text t2{4, 5, "BBB"};

    std::swap(t1, t2);

    REQUIRE(t1.text() == "BBB"s);
    REQUIRE(t2.text() == "AAA"s);
}