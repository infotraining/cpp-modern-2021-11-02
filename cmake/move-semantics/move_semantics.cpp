#include "catch.hpp"
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("lvalue vs. rvalue")
{
    int x;
    x = 10;

    // std::string{} = "text";
    auto text = std::string {};
}

string full_name(const string& first_name, const string& last_name)
{
    return first_name + " " + last_name;
}

namespace Explain
{
    template <typename T>
    class vector
    {
    public:
        void push_back(const T& item)
        {
            std::cout << "item " << item << " is copied into a vector\n";
        }

        void push_back(T&& item)
        {
            std::cout << "item " << item << " is moved into a container\n";
        }
    };
}

TEST_CASE("using push_back")
{
    Explain::vector<std::string> vec;

    std::string word = "one"s;

    vec.push_back(word);
    vec.push_back(std::string("two"));
    vec.push_back("three");
    vec.push_back("four"s);
    vec.push_back(std::move(word));
}

TEST_CASE("reference binding")
{
    string name = "jan";

    SECTION("C++98")
    {
        string& ref_name = name;

        const string& ref_full_name = full_name(name, "kowalski");
    }

    SECTION("C++11")
    {
        full_name(name, "kowalski");

        string&& ref_full_name = full_name(name, "kowalski");
        ref_full_name[0] = 'p';

        std::cout << ref_full_name << "\n";

        // string&& ref_name = name; // ERROR - cannot bind lvalue to rvalue ref
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

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) // move constructor
        : ptr_ {other.ptr_}
    {
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) // move assignment
    {
        if (this != &other)
        {
            delete ptr_; // release previous state

            // transfer state from other to this
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    ~UniquePtr()
    {
        delete ptr_;
    }

    explicit UniquePtr(T* ptr)
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
        cout << "Gadget(" << value << ", " << name << ")\n";
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
    UniquePtr<Gadget> pg1 {new Gadget {1, "ipad"}};
    pg1->use();

    pg1 = UniquePtr<Gadget>{new Gadget(2, "smartwatch")}; // move assignment

    UniquePtr<Gadget> pg2 = std::move(pg1); // move constructor
    pg2->use();
}