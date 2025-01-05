#include <iostream>
#include <thread>

void hello()
{
    std::cout << "Hello Concurrent World\n";
}

int main()
{
    std::jthread t(hello);
    // No need to call t.join() explicitly
}