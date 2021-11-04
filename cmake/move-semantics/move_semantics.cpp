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

UniquePtr<Gadget> create_gadget()
{
    static int id_gen = 0;

    auto ptr = UniquePtr<Gadget> {new Gadget(++id_gen)};

    return ptr;
}

void use_and_destroy(UniquePtr<Gadget> ptr)
{
    ptr->use();
}

TEST_CASE("move semantics - UniquePtr")
{
    UniquePtr<Gadget> pg1 {new Gadget {1, "ipad"}};
    pg1->use();

    pg1 = UniquePtr<Gadget> {new Gadget(2, "smartwatch")}; // move assignment

    UniquePtr<Gadget> pg2 = std::move(pg1); // move constructor
    pg2->use();

    UniquePtr<Gadget> pg3 = create_gadget();
    pg3->use();

    use_and_destroy(std::move(pg3));

    use_and_destroy(create_gadget());
}

class DataSet
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    DataSet(const std::string& name, std::initializer_list<int> list)
        : name_ {name}
        , size_ {list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);
    }

    DataSet(const DataSet& other)
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "DataSet(cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    DataSet& operator=(const DataSet& other)
    {
        if (this != &other)
        {
            delete[] data_;

            name_ = other.name_;
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.begin(), other.end(), data_);

            std::cout << "DataSet=(cc)\n";
        }

        return *this;
    }

    DataSet(DataSet&& other)
        : name_ {std::move(other.name_)}
        , data_ {other.data_}
        , size_ {other.size_}
    {
        other.size_ = 0;
        other.data_ = nullptr;

        std::cout << "DataSet(mv)\n";
    }

    DataSet& operator=(DataSet&& other)
    {
        if (this != &other)
        {
            delete[] data_;

            name_ = std::move(other.name_);

            size_ = other.size_;
            other.size_ = 0;

            data_ = other.data_;
            other.data_ = nullptr;

            std::cout << "DataSet=(mv)\n";
        }
        return *this;
    }

    ~DataSet()
    {
        delete[] data_;
    }

    iterator begin()
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }
};

DataSet create_data_set()
{
    DataSet ds {"data-set-one", {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

namespace LegacyCode
{
    DataSet* create_data_set()
    {
        DataSet* ds = new DataSet {"data-set-one", {54, 6, 34, 235, 64356, 235, 23}};

        return ds;
    }
}

TEST_CASE("DataSet")
{
    DataSet ds1 {"ds1", {1, 2, 3, 4, 5}};

    DataSet backup = ds1; // copy

    for (const auto& item : backup)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";

    DataSet ds2 = create_data_set();

    DataSet target = std::move(ds2);

    for (const auto& item : target)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";

    const DataSet const_ds {"const", {1, 2, 3}};
    DataSet ds3 = std::move(const_ds);
}

namespace AlternativeTake
{
    class DataSet
    {
        std::string name_;
        std::vector<int> data_;

    public:
        DataSet() = default;

        DataSet(const std::string& name, std::initializer_list<int> list)
            : name_ {name}
            , data_ {list}
        {
        }

        // DataSet(const DataSet& ) = default;
        // DataSet& operator=(const DataSet&) = default;

        // DataSet(DataSet&&) = default;
        // DataSet& operator=(DataSet&&) = default;

        // ~DataSet() {}

        std::string name() const
        {
            return name_;
        }

        using iterator = std::vector<int>::iterator;
        using const_iterator = std::vector<int>::const_iterator;

        iterator begin()
        {
            return data_.begin();
        }

        iterator end()
        {
            return data_.end();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        const_iterator end() const
        {
            return data_.end();
        }
    };
}

TEST_CASE("default copy & move")
{
    AlternativeTake::DataSet ds{"ds", {1, 2, 3, 4, 5}};

    //AlternativeTake::DataSet backup = ds; // copy

    AlternativeTake::DataSet target = std::move(ds);

    REQUIRE(ds.name() == ""s);

    static_assert(std::is_copy_constructible_v<DataSet>);
    static_assert(std::is_move_constructible_v<UniquePtr<Gadget>>);
}