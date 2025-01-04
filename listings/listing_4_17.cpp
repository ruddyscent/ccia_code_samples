#include <future>
#include <thread>
#include <type_traits>
#include <iostream>

template<typename Func>
std::future<decltype(std::declval<Func>()())>
spawn_async(Func&& func) {
    using result_type = decltype(std::declval<Func>()());
    std::promise<result_type> p;
    auto res = p.get_future();
    std::thread t(
        [p = std::move(p), f = std::forward<Func>(func)]() mutable {
            try {
                p.set_value_at_thread_exit(f());
            } catch (...) {
                p.set_exception_at_thread_exit(std::current_exception());
            }
        }
    );
    t.detach();
    return res;
}

int main() {
    auto task = []() { return 42; };
    std::future<int> result = spawn_async(task);
    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}