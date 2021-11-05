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

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

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

template <typename T, typename... TArgs>
UniquePtr<T> MakeUnique(TArgs&&... args)
{
    return UniquePtr<T>{new T(std::forward<TArgs>(args)...)};
}

TEST_CASE("move semantics - UniquePtr")
{
    UniquePtr<Gadget> pg1 = MakeUnique<Gadget>(1, "ipad");
    pg1->use();

    pg1 = UniquePtr<Gadget> {new Gadget(2, "smartwatch")}; // move assignment

    UniquePtr<Gadget> pg2 = std::move(pg1); // move constructor
    pg2->use();

    UniquePtr<Gadget> pg3 = create_gadget();
    pg3->use();

    use_and_destroy(std::move(pg3));

    use_and_destroy(create_gadget());
}

////////////////////////////////////////////////////////////
// DataSet - class with copy & move semantics (user provided implementation)

class DataSet
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    DataSet(std::string name, std::initializer_list<int> list)
        : name_ {std::move(name)}
        , size_ {list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);

        std::cout << "DataSet(" << name_ << ")\n";
    }

    DataSet(const DataSet& other)
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "DataSet(" << name_ << ": cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    void swap(DataSet& other)
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    DataSet& operator=(const DataSet& other)
    {
        DataSet temp(other);
        swap(temp);

        std::cout << "DataSet=(" << name_ << ": cc)\n";

        return *this;
    }

    DataSet(DataSet&& other)
        : name_ {std::move(other.name_)}
        , data_ {other.data_}
        , size_ {other.size_}
    {
        other.size_ = 0;
        other.data_ = nullptr;

        std::cout << "DataSet(" << name_ << ": mv)\n";
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

            std::cout << "DataSet=(" << name_ << ": mv)\n";
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

namespace ValueSemanticsTake
{
    class DataSet
    {
        std::string name_;
        std::vector<int> data_;

    public:
        DataSet() = default;

        DataSet(std::string name, std::initializer_list<int> list)
            : name_ {std::move(name)}
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
    ValueSemanticsTake::DataSet ds {"ds", {1, 2, 3, 4, 5}};

    ValueSemanticsTake::DataSet backup = ds; // copy

    ValueSemanticsTake::DataSet target = std::move(ds); // explicit move

    REQUIRE(ds.name() == ""s);

    static_assert(std::is_copy_constructible_v<DataSet>);
    static_assert(std::is_move_constructible_v<UniquePtr<Gadget>>);
}

template <typename T>
class Queue
{
    std::deque<T> q_;

public:
    Queue() = default;

    void push(const T& item)
    {
        q_.push_front(item);
    }

    void push(T&& item)
    {
        q_.push_front(std::move(item));
    }

    // template <typename TArg>
    // void push(TArg&& item)
    // {
    //     q_.push_front(std::forward<TArg>(item));
    // }

    template <typename... TArgs>
    void emplace(TArgs&&... args)
    {
        q_.emplace_front(std::forward<TArgs>(args)...);
    }
};

TEST_CASE("Queue & move semantics")
{
    std::cout << "\n\n\n--------------\n";

    Queue<DataSet> q;
    std::string name = "ds1";
    DataSet ds1 {std::move(name), {1, 2, 4, 3}};
    q.push(ds1); // copy
    q.push(DataSet {"ds2", {543, 645, 64, 665}}); // implicit move
    q.push(std::move(ds1)); // explicit move

    Queue<Gadget> qg;
    qg.push(Gadget{1, "ipad"s});
    qg.emplace(1, "ipad"s);
}

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    puts(__FUNCSIG__);
    g.use();
}

void have_fun(const Gadget& cg)
{
    puts(__FUNCSIG__);
    cg.use();
}

void have_fun(Gadget&& g)
{
    puts(__FUNCSIG__);
    g.use();
}

// void use(Gadget& g)
// {
//     have_fun(g);
// }

// void use(const Gadget& g)
// {
//     have_fun(g);
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g));
// }

namespace Explain
{
    template <typename T1, typename T2>
    using IsLike = std::is_same<std::remove_cv_t<std::remove_reference_t<T1>>, T2>;

    template <typename T1, typename T2>
    constexpr bool IsLike_v = IsLike<T1, T2>::value;

    template <typename TArg, typename = enable_if_t<IsLike_v<TArg, Gadget>>>
    void use(TArg&& arg)
    {
        have_fun(std::forward<TArg>(arg));
    }
}

template <typename TArg>
void use(TArg&& arg)
{
    have_fun(std::forward<TArg>(arg));
}

TEST_CASE("using gadget")
{
    Gadget g {1, "g"};
    const Gadget cg {2, "const g"};

    use(g);
    use(cg);
    use(Gadget {3, "temp g"});
}

///////////////
// reference collapsing

template <typename T>
void foo(T& arg)
{
    puts(__FUNCSIG__);
}

// TEST_CASE("ref collapsing")
// {
//     int x;
//     foo<int&>(x);
// }

TEST_CASE("auto&&")
{
    int x = 10;

    auto&& a1 = x; // int&

    auto&& a2 = 10; // int&&
}