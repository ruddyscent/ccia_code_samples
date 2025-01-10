#include <iostream>
#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
    std::scoped_lock guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find)
{
    std::scoped_lock guard(some_mutex);
    return std::ranges::find(some_list, value_to_find) != some_list.end();
}

int main()
{
    add_to_list(42);
    std::cout << std::boolalpha; // Enable boolean alpha output
    std::cout << "contains(1)=" << list_contains(1) << ", contains(42)=" << list_contains(42) << std::endl;
}
