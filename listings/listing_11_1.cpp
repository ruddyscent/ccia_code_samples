#include <future>
#include <cassert>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue {
private:
    std::queue<T> data_queue;
    mutable std::mutex mtx;
    std::condition_variable data_cond;

public:
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mtx);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        T value = std::move(data_queue.front());
        data_queue.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mtx);
        return data_queue.empty();
    }
};

void test_concurrent_push_and_pop_on_empty_queue() {
    threadsafe_queue<int> q;

    std::promise<void> go, push_ready, pop_ready;
    std::shared_future<void> ready(go.get_future());

    std::future<void> push_done;
    std::future<int> pop_done;

    try {
        push_done = std::async(std::launch::async,
                               [&q, ready, &push_ready]() {
                                   push_ready.set_value();
                                   ready.wait();
                                   q.push(42);
                               }
        );
        pop_done = std::async(std::launch::async,
                              [&q, ready, &pop_ready]() {
                                  pop_ready.set_value();
                                  ready.wait();
                                  return q.pop();
                              }
        );
        push_ready.get_future().wait();
        pop_ready.get_future().wait();
        go.set_value();

        push_done.get();
        assert(pop_done.get() == 42);
        assert(q.empty());
    } catch(...) {
        go.set_value();
        throw;
    }
}

int main() {
    test_concurrent_push_and_pop_on_empty_queue();
    return 0;
}