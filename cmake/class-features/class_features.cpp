#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

struct Person
{
    inline static int id_gen{};
    
    int id{++id_gen};
    uint8_t age{};
    std::string name{"not-set"};

    Person() = default;
    explicit Person(uint8_t age) : age{age}
    {}
    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;
};

template <typename T>
int calculate(T arg) = delete;

template<>
int32_t calculate(int32_t x)
{
    std::cout << "calculate(x: " << x << ")\n";

    return x;
}

TEST_CASE("default & delete")
{
    Person p1{42};
    REQUIRE(p1.name == "not-set"s);


    //Person p2 = p1; // Person is non-copyable

    calculate(42);
    //calculate(3.14);
    //calculate(42ULL);
}


class IGadget
{
public:
    virtual void use() const = 0;
    virtual ~IGadget() = default;
};

class Gadget : public IGadget
{
    int id_ {-1};
    std::string name_ = "unknown";

public:
    Gadget() = default;

    Gadget(int id, std::string name)
        : id_{id}
        , name_{std::move(name)}
    {
    }

    Gadget(int id) : Gadget{id, "not-set"s} // delegation to another constructor
    {
        name_.append("!!!");
    }

    int id() const
    {
        return id_;
    }

    std::string name() const
    {
        return name_;
    }

    void use() const override
    {
        std::cout << "Using gadget: " << id() << " - " << name() << "\n";
    }
};

class SuperGadget : public Gadget
{
public:
    using Gadget::Gadget; // inheritance of all constructors

    SuperGadget(int id)
        : Gadget {id, "not-set(super gadget)"}
    {
    }

    void use() const override final
    {
        std::cout << "Using super gadget: " << id() << " - " << name() << "\n";
    }
};

class HyperGadget final : public SuperGadget
{
public:
    using SuperGadget::SuperGadget;

    HyperGadget(int) = delete;

    // void use() const override
    // {
    //     std::cout << "Using hyper gadget: " << id() << " - " << name() << "\n";
    // }
};

//class IllegalGadget : HyperGadget{};

TEST_CASE("default constructor")
{
    Gadget g{};
}

TEST_CASE("inheritance of constructors")
{
    SuperGadget sg1 {1, "super-gadget"};
    SuperGadget sg2 {4};

    Gadget& g_ref = sg1;
    g_ref.use();
}