#include <chrono>
#include <deque>
#include <cstddef>
#include <iostream>
#include <vector>
#include <optional>
#include <thread>

template <typename T>
class stack
{
private:
    std::deque<T> data;  // Internal data container

public:
    stack() = default;

    bool empty() const
    {
        return data.empty();
    }

    void push(const T& value)
    {
        data.push_back(value);
    }

    void pop()
    {
        if (!data.empty())
            data.pop_back();
    }

    std::optional<T> top() const
    {
        if (!data.empty()) {
            // Simulate delay to demonstrate potential race condition
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return data.back();
        }
        return std::nullopt;  // Return nullopt if the stack is empty
    }
};

// Function to pop a value from the stack
void pop_from_stack(stack<int>& s)
{
    if (!s.empty())  // Check for empty() before calling top() (demonstrates race condition)
    {
        auto value = s.top();  // Another thread might call pop() here
        if (value)
        {
            s.pop();
            std::cout << "Popped: " << *value << std::endl;
        }
    }
}

int main()
{
    stack<int> s;

    // Add initial values to the stack
    for (int i = 0; i < 5; ++i)
    {
        s.push(i);
    }

    // Use threads to demonstrate concurrent pop operations
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back(pop_from_stack, std::ref(s));  // Pop from stack in separate threads
    }

    return 0;
}