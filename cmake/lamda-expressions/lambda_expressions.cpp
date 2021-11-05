#include "catch.hpp"
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <queue>

using namespace std;
using namespace Catch::Matchers;

class Lambda_4237846238746
{
public:
    auto operator()() const { std::cout << "Hello from lambda!!!\n"; }
};

TEST_CASE("lambda expressions")
{
    auto hello_lambda = []()
    { std::cout << "Hello from lambda!!!\n"; };

    hello_lambda();
    hello_lambda();

    SECTION("is interpreted as")
    {
        auto hello_lambda = Lambda_4237846238746 {};
        hello_lambda();
    }

    auto add = [](int32_t a, int32_t b) -> int64_t
    { return a + b; };

    REQUIRE(add(1, 2) == 3);

    SECTION("closure type")
    {
        // auto compare_by_pointed_value = [](const std::unique_ptr<int>& a, const std::unique_ptr<int>& b)
        // { return *a < *b; };

        auto compare_by_pointed_value = [](const auto& a, const auto& b)
        { return *a < *b; };

        std::set<std::unique_ptr<int>, decltype(compare_by_pointed_value)> up_set(compare_by_pointed_value);
        up_set.insert(std::make_unique<int>(13));
        up_set.insert(std::make_unique<int>(5));
        up_set.insert(std::make_unique<int>(1));

        for (const auto& ptr : up_set)
            std::cout << *ptr << " ";
        std::cout << "\n";

        std::set<std::shared_ptr<int>, decltype(compare_by_pointed_value)> sp_set(compare_by_pointed_value);
        sp_set.insert(std::make_unique<int>(13));
        sp_set.insert(std::make_unique<int>(5));
        sp_set.insert(std::make_unique<int>(1));

        for (const auto& ptr : sp_set)
            std::cout << *ptr << " ";
        std::cout << "\n";
    }
}

class Lambda_4283746237864
{
    const int factor_;

    Lambda_4283746237864(int f)
        : factor_ {f}
    {
    }

public:
    auto operator()(int x) const { return x * factor_; }
};

TEST_CASE("captures")
{
    int factor = 10;

    SECTION("by value - [=]")
    {
        auto multiply_by_factor = [=](int x)
        { return x * factor; };

        REQUIRE(multiply_by_factor(3) == 30);

        factor = 20;

        REQUIRE(multiply_by_factor(3) == 30);
    }

    SECTION("by ref - [&]")
    {
        auto increase = [&](int x) { factor += x; };

        increase(5);

        REQUIRE(factor == 15);
    }

    SECTION("explicit captures")
    {
        int sum{};

        auto calc = [factor, &sum](int x) { sum += x * factor; };
        
        calc(10);
        REQUIRE(sum == 100);
    }
}

struct Filter
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7};
    int threshold_ = 4;

    void filter()
    {
        auto increase_threshold = [this] { ++threshold_; };
        increase_threshold();

        auto new_end = std::remove_if(begin(vec), end(vec), [self = std::as_const(*this)](int item) { return item > self.threshold_; });
        vec.erase(new_end, end(vec));
    }
};

TEST_CASE("filter")
{
    Filter f{};
    f.filter();

    REQUIRE(f.vec == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("init capture - move semantics")
{
    std::unique_ptr<std::string> ptr = std::make_unique<std::string>("text");

    auto printer = [ptr = std::move(ptr)] { std::cout << *ptr << "\n"; };

    printer();
}

class Lambda_9132874912364275427
{
    int x_;

public:
    auto operator()() { return ++x_; }
};


TEST_CASE("closures are immutable")
{
    int x = 10;

    auto generate_number = [x]() mutable { return ++x; };

    REQUIRE(generate_number() == 11);
    REQUIRE(generate_number() == 12);
}

auto create_generator(int seed, int dx = 1)
{
    auto gen = [seed, dx]() mutable { 
        seed += dx;
        return seed; 
    };

    return gen;
}

TEST_CASE("generator")
{
    auto gen1 = create_generator(100);

    REQUIRE(gen1() == 101);
    REQUIRE(gen1() == 102);
    REQUIRE(gen1() == 103);
}

class Lambda_87234682736487236
{
public:
    template <typename T1, typename T2>
    auto operator()(T1 a, T2 b) const { return a + b; }
};

TEST_CASE("generic lambdas")
{
    auto add = [](auto a, auto b) { return a + b; };

    REQUIRE(add(1, 2) == 3);
    REQUIRE(add("aa"s, "bb"s) == "aabb"s);
}

TEST_CASE("storing & passing of closures")
{
    SECTION("storing")
    {
        SECTION("auto - preferred")
        {
            auto l = [] { return 42; };
            l();
        }

        SECTION("function pointers - for []")
        {
            int (*ptr_fun)(int) = [](int x) { return x * 2; };
            REQUIRE(ptr_fun(2) == 4);
        }

        SECTION("std::function")
        {
            int factor = 10;
            std::function<int(int, int)> f = [factor](int a, int b) { return factor * (a + b); };

            REQUIRE(f(2, 3) == 50);
        }
    }
}

struct Printer
{
    void print(const std::string& msg)
    {
        std::cout << "Printing " << msg << "\n";
    }
};

using Task = std::function<void()>;

class TaskQueue
{
    std::queue<Task> q_;
public:
    template <typename Callable>
    void submit(Callable&& callable)
    {
        q_.push(std::forward<Callable>(callable));
    }

    void run()
    {
        while(!q_.empty())
        {
            q_.front()();
            q_.pop();
        }
    }
};

TEST_CASE("using TaskQueue")
{
    TaskQueue tasks;

    Printer prn;

    Task printer = [&prn] { prn.print("Text"); };
    tasks.submit([] { std::cout << "Start...\n"; });
    tasks.submit(printer);
    tasks.submit([] { std::cout << "End...\n"; });

    // later

    tasks.run();
}

TEST_CASE("lambda & std algorithms")
{
    vector<int> vec = { 1, 43, 2532, 42, 665, 4234, 5436, 456 };

    std::for_each(begin(vec), end(vec), [](const auto& item) { std::cout << item << " "; });
    std::cout << "\n";

    int threshold = 500;

    auto pos = std::find_if(begin(vec), end(vec), [threshold](int x) { return x > threshold; });
    REQUIRE(*pos == 2532);

    vector<string> words = { "Ala", "ma", "kokarde", "!"};

    auto compare_by_length = [](const auto& a, const auto& b) { return a.size() < b.size(); };
    std::sort(begin(words), end(words), compare_by_length);

    for(const auto& word : words)
        cout << word << " ";
    cout << "\n"; 
}

TEST_CASE("using lambda to initialize const")
{
    int x = 12;

    const string description = [x]() { 
        if (x % 2 == 0) 
            return "even"s;
        else if (x % 3)
            return "odd but divisible by 3"s;
        else
            return "other"s;
    }();

    REQUIRE(description == "even"s);
}

TEST_CASE("Since C++17")
{
    auto add = [](int a, int b) { return a + b; };

    constexpr int x = 10;
    static_assert(add(1, x) == 11, "Error");

}