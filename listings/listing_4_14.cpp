#include <future>
#include <type_traits>
#include <thread>
#include <iostream>

template<typename F, typename A>
std::future<typename std::invoke_result<F, A&&>::type>
spawn_task(F&& f, A&& a)
{
    using result_type = typename std::invoke_result<F, A&&>::type;
    std::packaged_task<result_type(A&&)> task(std::move(f));
    std::future<result_type> res = task.get_future();
    std::thread t(std::move(task), std::move(a));
    t.detach();
    return res;
}

int main()
{
    auto task = [](int x) { return x * 2; };
    std::future<int> result = spawn_task(task, 5);
    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}