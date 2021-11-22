#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <list>
#include <algorithm>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() {}
};

class Subject
{
    int state_;
    std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

public:
    Subject() : state_(0)
    {
    }

    void register_observer(std::weak_ptr<Observer> observer)
    {
        observers_.insert(std::move(observer));
    }

    void unregister_observer(std::weak_ptr<Observer> observer)
    {
        observers_.erase(std::move(observer));
    }

    void set_state(int new_state)
    {
        if (state_ != new_state)
        {
            state_ = new_state;
            notify("Changed state on: " + std::to_string(state_));
        }
    }

protected:
    void notify(const std::string& event_args)
    {
        auto it = begin(observers_);
        for(auto it = begin(observers_); it != end(observers_);)
        {
            if (auto target = it->lock())
            {
                target->update(event_args);
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }
};

class ConcreteObserver1 
    : public Observer, public std::enable_shared_from_this<ConcreteObserver1>
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver1: " << event << std::endl;
    }

    void register_me(Subject& s)
    {
        s.register_observer(shared_from_this());
    }
};

class ConcreteObserver2 : public Observer
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver2: " << event << std::endl;
    }

    void extra_stuff()
    {
        std::cout << "ConcreteObserver2::extra_stuff()\n";
    }
};

int main(int argc, char const* argv[])
{
    using namespace std;

    Subject s;

    auto o1 = make_shared<ConcreteObserver1>();
    o1->register_me(s);

    {
        std::shared_ptr<Observer> o2 = make_shared<ConcreteObserver2>();
        s.register_observer(o2);

        if (auto temp = std::dynamic_pointer_cast<ConcreteObserver2>(o2))
            temp->extra_stuff();

        s.set_state(1);

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}
