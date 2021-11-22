#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <vector>
#include <array>

using namespace std;

TEST_CASE("lambda exercise")
{
    using namespace Catch::Matchers;

    vector<int> data = {1, 6, 3, 5, 8, 9, 13, 12, 10, 45};

    auto is_even =  [](const auto& val) { return val % 2 == 0; };

    SECTION("count even numbers")
    {
        auto evens_count = 0;

        evens_count = std::count_if(cbegin(data), cend(data), is_even);
        
        REQUIRE(evens_count == 4);
    }

    SECTION("copy evens to vector")
    {
        vector<int> evens;

        std::copy_if(cbegin(data), cend(data), std::back_insert_iterator(evens), is_even);

        REQUIRE_THAT(evens, Equals(vector<int>{6, 8, 12, 10}));
    }

    SECTION("create container with squares")
    {
        vector<int> squares;
        squares.reserve(std::size(squares));
        
        //auto square = [](int x) { return x * x; };               
        //std::transform(cbegin(data), cend(data), std::back_inserter(squares), square);

        std::transform(cbegin(data), cend(data), cbegin(data), std::back_inserter(squares), std::multiplies{}); // -> map

        REQUIRE_THAT(squares, Equals(vector<int>{1, 36, 9, 25, 64, 81, 169, 144, 100, 2025}));
    }

    SECTION("remove from container items divisible by any number from a given array")
    {
        const array<int, 3> eliminators = {3, 5, 7};

         auto erase_from = std::remove_if(begin(data), end(data), [=](auto item){ 
            return std::any_of(eliminators.begin(), eliminators.end(), [=](auto eliminator){ return item % eliminator == 0;});
        });

        data.erase(erase_from, data.end());

        REQUIRE_THAT(data, Equals(vector<int>{1, 8, 13}));
    }

    SECTION("calculate average")
    {
        double sum = std::accumulate(std::cbegin(data), std::cend(data), 0.0);
        double avg = sum / std::size(data);

        REQUIRE(avg == Approx(11.2));

        SECTION("create two containers - 1st with numbers less or equal to average & 2nd with numbers greater than average")
        {
            vector<int> less_equal_than_avg;
            vector<int> greater_than_avg;

            std::partition_copy(cbegin(data), cend(data), 
                                std::back_inserter(less_equal_than_avg), // where_true
                                std::back_inserter(greater_than_avg), // where_false
                                [avg](int val) { return val <= avg; });
            
            REQUIRE_THAT(less_equal_than_avg, Contains(vector<int>{1, 6, 3, 5, 8, 9, 10}));
            REQUIRE_THAT(greater_than_avg, Contains(vector<int>{13, 12, 45}));
        }
    }

    SECTION("find 5 the greatest numbers in data")
    {
        std::nth_element(begin(data), begin(data) + 5, end(data), std::greater{});

        std::vector<int> the_greatest(5);

        std::copy_n(cbegin(data), 5, begin(the_greatest));
    }
}
