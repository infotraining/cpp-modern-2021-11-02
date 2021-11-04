#include "catch.hpp"
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

string full_name(const string& first_name, const string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    string name = "jan";

    SECTION("C++98")
    {
        // TODO
    }

    SECTION("C++11")
    {
        // TODO
    }
}

template <typename T>
class UniquePtr
{
    T* ptr_;

public:
    UniquePtr(nullptr_t)
        : ptr_ {nullptr}
    {
    }

    ~UniquePtr()
    {
        delete ptr_;
    }

    // TODO: implement move semantics

    explicit UniquePtr(T* ptr = nullptr)
        : ptr_ {ptr}
    {
    }

    explicit operator bool() const
    {
        return ptr_ != nullptr;
    }

    
    T* get() const
    {
        return ptr_;
    }

    T* operator->() const
    {
        return ptr_;
    }

    T& operator*() const
    {
        return ptr_;
    }
};

struct Gadget
{
    int value {};
    std::string name {};

    Gadget() = default;

    Gadget(int v)
        : value {v}
    {
        cout << "Gadget(" << value << ")\n";
    }

    Gadget(int v, const std::string& n)
        : value {v}
        , name {n}
    {
    }

    void use() const
    {
        cout << "Using Gadget(" << value << ")\n";
    }

    ~Gadget()
    {
        cout << "~Gadget(" << value << ")\n";
    }
};

TEST_CASE("move semantics - UniquePtr")
{
    // TODO
    UniquePtr<Gadget> pg1{new Gadget{1, "ipad"}};
    pg1->use();
}