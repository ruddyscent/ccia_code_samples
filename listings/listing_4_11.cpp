#include <condition_variable>
#include <mutex>
#include <chrono>

std::condition_variable cv;
bool done = false;
std::mutex m;

bool wait_loop()
{
    auto const timeout = std::chrono::steady_clock::now() +
        std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lk(m);
    while(!done)
    {
        if(cv.wait_until(lk, timeout) == std::cv_status::timeout)
            break;
    }
    return done;
}

int main()
{
    // Example usage of wait_loop
    bool result = wait_loop();
    return result ? 0 : 1;
}