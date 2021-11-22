#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <thread>
#include <future>

#include "catch.hpp"

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

using namespace std;
using namespace Utils;

namespace [[deprecated]] LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
}

TEST_CASE("Legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    // {
    //     use_gadget(get_gadget("ipad"));
    // } // memory leak

    // {
    //     Gadget* g = get_gadget("ipad");

    //     use(g);

    //     std::cout << g->name() << std::endl; // UB - use after delete
    // }
}

////////////////////////////////////////////////
// Modern C++

namespace ModernCpp
{
    // forward declarations
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    void use(std::unique_ptr<Gadget> g);
    void use_gadget(Gadget* g);

    // definitions
    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        return std::make_unique<Gadget>(++id, name);
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget& g)
    {
        std::cout << "Using " << g.name() << "\n";
    }
}

TEST_CASE("Modern C++")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());

        if (g)
            use_gadget(*g);

        g.reset();

        assert(g == nullptr);
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("smartwatch");
        use_gadget(g.get());

        LegacyCode::use(g.release());

        REQUIRE(g == nullptr);
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");
        use(std::move(g));

        use(get_gadget("iwatch"));
    }
}

class Owner
{
    std::string name_;
    std::unique_ptr<Gadget> gadget_;

public:
    Owner(std::string name, std::unique_ptr<Gadget> gadget)
        : name_ {std::move(name)}
        , gadget_ {std::move(gadget)}
    {
    }

    void use() const
    {
        if (gadget_)
            std::cout << name_ << " is using " << gadget_->name() << "\n";
    }
};

TEST_CASE("Owner")
{
    Owner o1 {"Jan Kowalski", ModernCpp::get_gadget("ipad")};

    Owner o2 = std::move(o1);

    o2.use();
}

TEST_CASE("using unique_ptr to control FILE")
{
    SECTION("Legacy code")
    {
        FILE* f = fopen("data.txt", "w+");

        fprintf(f, "text");
        // may_throw()

        fclose(f);
    }

    SECTION("Modern C++")
    {
        std::unique_ptr<FILE, int (*)(FILE*)> f {fopen("data.txt", "w+"), &fclose};

        fprintf(f.get(), "text");
        // may_throw()
    }
}

struct Stream
{
    std::string name;

    void send() { }

    void close()
    {
        std::cout << "Closing stream " << name << "\n";
    }
};

template <typename TResource, typename TDeallocator>
[[nodiscard]] auto make_raii(TResource* resource, TDeallocator deallocator)
{
    return std::unique_ptr<TResource, TDeallocator>(resource, deallocator);
}

TEST_CASE("Closing stream with unique_ptr")
{
    Stream s {"dev1"};

    // auto stream_closer = [](Stream* s) { s->close(); };
    // std::unique_ptr<Stream, decltype(stream_closer)> s_closer(&s, stream_closer);

    auto raii_closer = make_raii(&s, [](Stream* s)
        { s->close(); });

    s.send();
}

namespace LegacyCode
{
    int* make_array(size_t size)
    {
        return new int[size];
    }
}

TEST_CASE("std::unique_ptr<T[]>")
{
    std::unique_ptr<int[]> tab{LegacyCode::make_array(100)};

    tab[0] = 1;
}

std::string load_data(const std::string& filename)
{
    std::cout << "Start loading from " << filename << "\n";
    std::this_thread::sleep_for(2s);
    return "Content of " + filename;
}

void save_to_file(const std::string& filename)
{
    std::cout << "Start saving to " << filename << "\n";
    std::this_thread::sleep_for(2s);
    std::cout << "Saving to " << filename << " is finished\n";
}

// TEST_CASE("BUG in C++11 - async")
// {
//     [[maybe_unused]] int gen_id{};

//     std::future<std::string> fcontent1 = std::async(std::launch::async, load_data, "input1.txt");
//     std::future<std::string> fcontent2 = std::async(std::launch::async, load_data, "input2.txt");
//     std::future<std::string> fcontent3 = std::async(std::launch::async, load_data, "input3.txt");

//     auto f1 = std::async(std::launch::async, save_to_file, fcontent1.get());
//     auto f2 = std::async(std::launch::async, save_to_file, fcontent2.get());
//     auto f3 = std::async(std::launch::async, save_to_file, fcontent3.get());
// }

////////////////////////////////////////////
// shared pointers

TEST_CASE("shared pointers")
{
    auto sp1 = std::make_shared<Gadget>(665, "shared ipad");
    REQUIRE(sp1.use_count() == 1);
}