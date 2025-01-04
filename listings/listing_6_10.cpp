#include <memory>
#include <mutex>
#include <iostream>

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
    mutable std::mutex head_mutex;
    mutable std::mutex tail_mutex;

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

    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return std::unique_ptr<node>();
        }
        value = std::move(*head->data);
        return pop_head();
    }

public:
    threadsafe_queue() : head(new node), tail(head.get()) {}

    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> const old_head = try_pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    bool try_pop(T& value)
    {
        std::unique_ptr<node> const old_head = try_pop_head(value);
        return old_head != nullptr;
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

    bool empty() const
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head.get() == get_tail());
    }
};

int main()
{
    threadsafe_queue<int> q;
    q.push(42);
    std::shared_ptr<int> p = q.try_pop();
    if (p)
    {
        std::cout << "Popped value: " << *p << std::endl;
    }
    else
    {
        std::cout << "Queue is empty" << std::endl;
    }
    return 0;
}