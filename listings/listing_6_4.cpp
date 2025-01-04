#include <memory>
#include <iostream>
#include <thread>

template<typename T>
class queue
{
private:
    struct node
    {
        T data;
        std::unique_ptr<node> next;

        node(T data_):
            data(std::move(data_))
        {}
    };
    
    std::unique_ptr<node> head;
    node* tail;
    
public:
    queue():
        tail(nullptr)
    {}

    queue(const queue& other)=delete;
    queue& operator=(const queue& other)=delete;

    std::shared_ptr<T> try_pop()
    {
        if(!head)
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<node> const old_head=std::move(head);
        head=std::move(old_head->next);
        if(!head)
            tail=nullptr;
        return res;
    }
    
    void push(T new_value)
    {
        std::unique_ptr<node> p(new node(std::move(new_value)));
        node* const new_tail=p.get();
        if(tail)
        {
            tail->next=std::move(p);
        }
        else
        {
            head=std::move(p);
        }
        tail=new_tail;
    }
};

void producer(queue<int>& q)
{
    for (int i = 0; i < 10; ++i)
    {
        q.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(queue<int>& q)
{
    for (int i = 0; i < 10; ++i)
    {
        std::shared_ptr<int> value = q.try_pop();
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
    queue<int> q;
    std::thread prod(producer, std::ref(q));
    std::thread cons(consumer, std::ref(q));

    prod.join();
    cons.join();

    return 0;
}