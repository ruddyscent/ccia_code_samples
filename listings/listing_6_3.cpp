#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <thread>

template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue() {}

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        value = std::move(*data_queue.front());
        data_queue.pop();
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

    void push(T new_value)
    {
        std::shared_ptr<T> data(
            std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
};

void producer(threadsafe_queue<int>& queue)
{
    for (int i = 0; i < 10; ++i)
    {
        queue.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(threadsafe_queue<int>& queue)
{
    for (int i = 0; i < 10; ++i)
    {
        int value;
        queue.wait_and_pop(value);
        std::cout << "Consumed: " << value << std::endl;
    }
}

int main()
{
    threadsafe_queue<int> queue;
    std::thread prod(producer, std::ref(queue));
    std::thread cons(consumer, std::ref(queue));

    prod.join();
    cons.join();

    return 0;
}