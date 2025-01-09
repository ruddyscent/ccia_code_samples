#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <iostream>
#include <chrono>

void do_work(unsigned id)
{
    std::cout << "Thread " << id << " starting...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    std::cout << "Thread " << id << " finished.\n";
}

void f()
{
    std::vector<std::jthread> threads;
    threads.reserve(20);  // Preallocate space for better performance
    
    for (unsigned i = 0; i < 20; ++i)
    {
        threads.emplace_back(do_work, i);
    }
    // No need for explicit join with std::jthread
}

int main()
{
    f();
    return 0;
}