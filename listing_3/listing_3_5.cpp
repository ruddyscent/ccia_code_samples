#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <optional>
#include <iostream>

struct empty_stack : std::exception
{
    const char *what() const noexcept override
    {
        return "empty stack";
    }
};

template <typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    threadsafe_stack() = default;
    threadsafe_stack(const threadsafe_stack &other)
    {
        std::scoped_lock lock(other.m);
        data = other.data; // Copy performed in constructor body
    }
    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    void push(T new_value)
    {
        std::scoped_lock lock(m);
        data.push(std::move(new_value));
    }

    std::shared_ptr<T> pop()
    {
        std::scoped_lock lock(m);
        if (data.empty())
            throw empty_stack(); // Check for empty before trying to pop value
        std::shared_ptr<T> const res = std::make_shared<T>(std::move(data.top())); // Allocate return value before modifying stack
        data.pop();
        return res;
    }

    void pop(T &value)
    {
        std::scoped_lock lock(m);
        if (data.empty())
            throw empty_stack();
        value = std::move(data.top());
        data.pop();
    }

    std::optional<T> try_pop()
    {
        std::scoped_lock lock(m);
        if (data.empty())
            return std::nullopt;
        T value = std::move(data.top());
        data.pop();
        return value;
    }

    bool empty() const
    {
        std::scoped_lock lock(m);
        return data.empty();
    }
};

int main()
{
    threadsafe_stack<int> s;
    s.push(42);
    auto value = s.try_pop();
    if (value)
    {
        std::cout << "Popped value: " << *value << std::endl;
    }
    else
    {
        std::cout << "Stack is empty" << std::endl;
    }
}