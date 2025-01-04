#include <memory>
#include <mutex>
#include <iostream>
#include <thread>

template<typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::unique_ptr<node> head;
    node* tail;
    std::mutex head_mutex;
    std::mutex tail_mutex;

    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    threadsafe_queue() : head(new node), tail(head.get()) {}

    threadsafe_queue(const threadsafe_queue& other) = delete;
    threadsafe_queue& operator=(const threadsafe_queue& other) = delete;

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return std::shared_ptr<T>();
        }
        return pop_head()->data;
    }

    void push(T new_value)
    {
        std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
        std::unique_ptr<node> p(new node);
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            node* const new_tail = p.get();
            tail->next = std::move(p);
            tail = new_tail;
        }
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
        std::shared_ptr<int> value = queue.try_pop();
        if (value)
        {
            std::cout << "Consumed: " << *value << std::endl;
        }
        else
        {
            std::cout << "Queue is empty" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
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